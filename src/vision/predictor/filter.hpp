#pragma once

#include <vector>

#include "common.hpp"
#include "opencv2/opencv.hpp"

class Filter {
 private:
  bool predict_condition_;

 public:
  unsigned int measurements_, states_;

  virtual const cv::Mat& Predict(const cv::Mat& measurements,
                                 const cv::Mat& frame) = 0;
};

class Kalman : public Filter {
 private:
  cv::KalmanFilter kalman_filter_;
  void Init(int states, int measurements);
  cv::Mat cur_predict_matx_, last_predict_matx_;
  cv::Mat cur_measure_matx_, last_measure_matx_;
  unsigned int error_frame_;

 public:
  Kalman();
  Kalman(int states, int measurements);
  ~Kalman();
  const cv::Point2d Predict(const cv::Point2d& measurements_point,
                            const cv::Mat& frame);
  const cv::Mat& Predict(const cv::Mat& measurements, const cv::Mat& frame);
};

class EKF : public Filter {
 public:
  typedef cv::Matx33d Matx33d;
  typedef cv::Matx<double, 3, 5> Matx35d;
  typedef cv::Matx<double, 5, 3> Matx53d;
  typedef cv::Matx<double, 5, 5> Matx55d;
  typedef cv::Vec3d Vec3d;
  typedef cv::Matx<double, 5, 1> Vec5d;

 private:
  Vec5d Xe;   // Vec5d state_pre_;                  /* 估计状态变量 */
  Vec5d Xp;   // Vec5d state_next_;                 /* 预测状态变量 */
  Matx55d F;  // Matx55d predict_mat_;              /* 预测雅克比 */
  Matx35d H;  // Matx35d measurement_mat_;          /* 观测雅克比 */
  Matx55d P;  // Matx55d state_cov_;                /* 状态协方差 */
  Matx55d Q;  // Matx55d process_noi_cov_mat_;      /* 预测过程协方差 */
  Matx33d R;  // Matx33d measurement_noi_cov_mat_;  /* 观测过程协方差 */
  Matx35d K;  // Matx35d kalman_gain_;              /* 卡尔曼增益 */
  Vec3d Yp;   // Vec3d predict_obs_;                /* 预测观测量 */

  void Init(const Vec5d& Xe);

 public:
  EKF();
  EKF(const Vec5d& Xe);
  ~EKF();
  const cv::Mat& Predict(const cv::Mat& measurements, const cv::Mat& frame);
};
