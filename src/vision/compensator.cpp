#include "compensator.hpp"

#include "opencv2/opencv.hpp"
#include "spdlog/spdlog.h"

namespace {

const double kG = 9.80665;
const cv::Scalar kGREEN(0., 255., 0.);
const cv::Scalar kRED(0., 0., 255.);
const cv::Scalar kYELLOW(0., 255., 255.);

}  // namespace

cv::Point3f Compensator::EstimateWorldCoord(Armor& armor) {
  cv::Mat rot_vec, trans_vec;
  cv::solvePnP(armor.ModelVertices(), armor.SurfaceVertices(), cam_mat_,
               distor_coff_, rot_vec, trans_vec, false, cv::SOLVEPNP_IPPE);
  armor.SetRotVec(rot_vec), armor.SetTransVec(trans_vec);
  cv::Mat world_coord =
      ((cv::Vec2f(armor.SurfaceCenter()) * cam_mat_.inv() - trans_vec) *
       armor.GetRotMat().inv());
  return cv::Point3f(world_coord);
}

/**
 * @brief Angle θ required to hit coordinate (x, y)
 *
 * {\displaystyle \tan \theta ={\left({\frac {v^{2}\pm {\sqrt
 * {v^{4}-g(gx^{2}+2yv^{2})}}}{gx}}\right)}}
 *
 * @param target 目标坐标
 * @return double 出射角度
 */
double Compensator::SolveSurfaceLanchAngle(cv::Point2f target) {
  const double v_2 = pow(ballet_speed_, 2);
  const double up_base =
      std::sqrt(std::pow(ballet_speed_, 4) -
                kG * (kG * std::pow(target.x, 2) + 2 * target.y * v_2));
  const double low = kG * target.x;
  const double ans1 = std::atan2(v_2 + up_base, low);
  const double ans2 = std::atan2(v_2 - up_base, low);

  if (std::isnan(ans1)) return std::isnan(ans2) ? 0. : ans2;
  if (std::isnan(ans2)) return std::isnan(ans1) ? 0. : ans1;
  return std::min(ans1, ans2);
}

void Compensator::VisualizePnp(Armor& armor, const cv::Mat& output,
                               bool add_lable) {
  std::vector<cv::Point2f> out_points;
  cv::projectPoints(armor.SurfaceVertices(), armor.GetRotVec(),
                    armor.GetTransVec(), cam_mat_, distor_coff_, out_points);
  for (std::size_t i = 0; i < out_points.size(); ++i) {
    cv::line(output, out_points[i], out_points[(i + 1) % out_points.size()],
             kGREEN);
  }
}

Compensator::Compensator() { SPDLOG_TRACE("Constructed."); }

Compensator::Compensator(const std::string& path) {
  SPDLOG_TRACE("Constructed.");
  LoadCameraMat(path);
}

Compensator::~Compensator() { SPDLOG_TRACE("Destructed."); }

void Compensator::LoadCameraMat(const std::string& path) {
  cv::FileStorage fs(path,
                     cv::FileStorage::READ | cv::FileStorage::FORMAT_JSON);

  if (fs.isOpened()) {
    cam_mat_ = fs["cam_mat"].mat();
    distor_coff_ = fs["distor_coff"].mat();
    if (cam_mat_.empty() && distor_coff_.empty()) {
      SPDLOG_ERROR("Can not load cali data.");
    } else {
      SPDLOG_DEBUG("Loaded cali data.");
    }
  } else {
    SPDLOG_ERROR("Can not open file: '{}'", path);
  }
}

void Compensator::Apply(std::vector<Armor>& armors) {
  for (auto& armor : armors) {
    auto coord = EstimateWorldCoord(armor);

    Euler aiming_eulr;
    cv::Point2f surface_target(
        std::sqrt(std::pow(coord.x, 2) + std::pow(coord.y, 2)), -coord.z);
    aiming_eulr.pitch = SolveSurfaceLanchAngle(surface_target);
    aiming_eulr.yaw = std::atan2(coord.x, coord.y);
    armor.SetAimEuler(aiming_eulr);
  }
}

void Compensator::VisualizeResult(std::vector<Armor>& armors,
                                  const cv::Mat& output, int verbose) {
  for (auto& armor : armors) {
    VisualizePnp(armor, output, verbose > 1);
  }
}
