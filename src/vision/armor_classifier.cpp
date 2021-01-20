#include "armor_classifier.hpp"

#include "opencv2/opencv.hpp"
#include "spdlog/spdlog.h"

ArmorClassifier::ArmorClassifier() {}
ArmorClassifier::~ArmorClassifier() {}

void ArmorClassifier::StoreModel(std::string path) {}
void ArmorClassifier::LoadModel(std::string path) {}

void ArmorClassifier::Train() {}

game::Model ArmorClassifier::ClassifyModel(Armor &armor) {
  return game::Model::kINFANTRY;
}

game::Team ArmorClassifier::ClassifyTeam(Armor &armor) {
  return game::Team::kBLUE;
}