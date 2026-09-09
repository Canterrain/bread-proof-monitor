#include "recipe_presets.h"

#include "outcome_feedback.h"
#include "text_utils.h"

namespace {
constexpr FermentationEvent kNoEvents[kMaxProfileEvents] = {};

const RecipePreset kVisibleRecipePresets[] = {
    {"Custom", "Custom", 75, 50, true, 0, {}},
    {"Everyday Bread", "Country Loaf", 90, 50, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/country-loaf-recipe"},
    {"Everyday Bread", "Sandwich Bread", 50, 75, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/classic-sandwich-bread-recipe"},
    {"Everyday Bread", "Whole Wheat Sandwich Bread", 50, 70, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/100-whole-wheat-sandwich-bread-recipe"},
    {"Everyday Bread", "French Bread", 90, 45, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/french-style-country-bread-recipe",
     nullptr,
     "Complete the recipe's brief rest and mixing steps first."},
    {"Everyday Bread", "Sourdough Sandwich Bread", 100, 60, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/sourdough-milk-bread-recipe"},
    {"Everyday Bread", "Rustic Sourdough", 90, 50, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/rustic-sourdough-bread-recipe"},
    {"Everyday Bread", "No-Knead Sourdough Bread", 75, 0, false, 2,
     {
         {"Bulk", "handling", "Stretch & Fold",
          "Stretch and fold the dough, return it under the monitor, then press Complete Step.",
          25, "Stretch & Fold", "Your dough is ready for its first stretch and fold."},
         {"Bulk", "handling", "Stretch & Fold",
          "Stretch and fold the dough again, return it under the monitor, then press Complete Step.",
          50, "Stretch & Fold", "Your dough is ready for its second stretch and fold."},
         {},
         {},
     },
     "https://www.kingarthurbaking.com/recipes/no-knead-sourdough-bread-recipe",
     "Do one more stretch and fold, then refrigerate the dough as-is for its long cold rest. "
     "When you're ready to bake, follow the recipe for shaping, a final room-temperature "
     "rise, and baking, instead of the monitor."},
    {"Everyday Bread", "Milk Bread", 100, 75, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/japanese-milk-bread-rolls-recipe",
     nullptr,
     "Complete the recipe's tangzhong and mixing steps first."},
    {"Everyday Bread", "Baguettes", 90, 50, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/sourdough-baguettes-recipe"},
    {"Pizza and Flatbread", "Pizza Dough", 50, 0, false, 0, {},
     "https://www.kingarthurbaking.com/recipes/pizza-crust-recipe",
     "Shape the dough, then follow the recipe's remaining rests and final rise instead of "
     "the monitor."},
    {"Pizza and Flatbread", "Neapolitan Pizza", 35, 0, false, 0, {},
     "https://www.kingarthurbaking.com/recipes/neapolitan-style-pizza-crust-recipe",
     "Divide and shape the dough, then follow the recipe's rest while the oven preheats "
     "instead of the monitor."},
    {"Pizza and Flatbread", "Detroit Pizza", 35, 35, true, 2,
     {
         {"Bulk", "handling", "Bowl Fold",
          "Fold the dough in the bowl, return it under the monitor, then press Complete Step.",
          10, "Bowl Fold", "Your dough is ready for its first bowl fold."},
         {"Bulk", "handling", "Bowl Fold",
          "Fold the dough in the bowl, return it under the monitor, then press Complete Step.",
          20, "Bowl Fold", "Your dough is ready for its second bowl fold."},
         {},
         {},
     },
     "https://www.kingarthurbaking.com/recipes/weeknight-detroit-pizza-recipe"},
    {"Pizza and Flatbread", "Sourdough Pizza Crust", 85, 0, false, 0, {},
     "https://www.kingarthurbaking.com/recipes/sourdough-pizza-crust-recipe",
     "Shape the dough into balls, then follow the recipe's refrigerated rest and warm-up "
     "instead of the monitor.",
     "Complete the recipe's levain build and mixing steps first."},
    {"Pizza and Flatbread", "Focaccia", 85, 60, true, 4,
     {
         {"Bulk", "handling", "Bowl Fold",
          "Fold the dough in the bowl, return it under the monitor, then press Complete Step.",
          10, "Bowl Fold", "Your dough is ready for its first bowl fold."},
         {"Bulk", "handling", "Bowl Fold",
          "Fold the dough in the bowl, return it under the monitor, then press Complete Step.",
          20, "Bowl Fold", "Your dough is ready for its second bowl fold."},
         {"Bulk", "handling", "Bowl Fold",
          "Fold the dough in the bowl, return it under the monitor, then press Complete Step.",
          30, "Bowl Fold", "Your dough is ready for its third bowl fold."},
         {"Bulk", "handling", "Bowl Fold",
          "Fold the dough in the bowl, return it under the monitor, then press Complete Step.",
          40, "Bowl Fold", "Your dough is ready for its fourth bowl fold."},
     },
     "https://www.kingarthurbaking.com/recipes/big-and-bubbly-focaccia-recipe",
     nullptr,
     nullptr,
     10},
    {"Pizza and Flatbread", "Sourdough Focaccia", 50, 0, false, 1,
     {
         {"Bulk", "handling", "Bowl Fold",
          "Fold the dough in the bowl, return it under the monitor, then press Complete Step.",
          25, "Bowl Fold", "Your dough is ready for its bowl fold."},
         {},
         {},
         {},
     },
     "https://www.kingarthurbaking.com/recipes/sourdough-focaccia-recipe",
     "Shape the dough into the pan, then follow the recipe's overnight refrigerated rise "
     "instead of the monitor."},
    {"Pizza and Flatbread", "Pita", 25, 0, false, 0, {},
     "https://www.kingarthurbaking.com/recipes/golden-pita-bread-recipe",
     "Shape into rounds, then follow the recipe's short rest before baking. The puff "
     "happens in the oven, not during this rest, so it isn't tracked here."},
    {"Pizza and Flatbread", "Naan", 100, 0, false, 0, {},
     "https://www.kingarthurbaking.com/recipes/naan-recipe",
     "Shape into balls, then follow the recipe's rest before cooking. The puff comes from "
     "the cooking heat, not from proofing, so it isn't tracked here."},
    {"Breakfast", "English Muffins", 50, 35, true, 2,
     {
         {"Bulk", "handling", "Bowl Fold",
          "Fold the dough in the bowl, return it under the monitor, then press Complete Step.",
          25, "Bowl Fold", "Your dough is ready for its first bowl fold."},
         {"Bulk", "handling", "Bowl Fold",
          "Fold the dough in the bowl again, return it under the monitor, then press Complete Step.",
          45, "Bowl Fold", "Your dough is ready for its second bowl fold."},
         {},
         {},
     },
     "https://www.kingarthurbaking.com/recipes/english-muffins-recipe",
     nullptr,
     "Complete the recipe's preferment and mixing steps first."},
    {"Breakfast", "Bagels", 50, 20, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/bagels-recipe"},
    {"Breakfast", "Sourdough Bagels", 40, 20, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/sourdough-bagels-recipe"},
    {"Breakfast", "Cinnamon Rolls", 65, 45, true, 3,
     {
         {"Bulk", "handling", "Stretch & Fold",
          "Stretch and fold the dough, return it under the monitor, then press Complete Step.",
          20, "Stretch & Fold", "Your dough is ready for its first stretch and fold."},
         {"Bulk", "handling", "Stretch & Fold",
          "Stretch and fold the dough again, return it under the monitor, then press Complete Step.",
          40, "Stretch & Fold", "Your dough is ready for its second stretch and fold."},
         {"Bulk", "handling", "Stretch & Fold",
          "Stretch and fold the dough again, return it under the monitor, then press Complete Step.",
          60, "Stretch & Fold", "Your dough is ready for its third stretch and fold."},
         {},
     },
     "https://www.kingarthurbaking.com/recipes/sourdough-cinnamon-rolls-recipe",
     nullptr,
     "Complete the recipe's autolyse and mixing steps first."},
    {"Breakfast", "Sticky Buns", 90, 50, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/sticky-buns-recipe"},
    {"Specialty", "Babka", 50, 50, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/chocolate-babka-recipe",
     nullptr,
     "Complete the recipe's rest and mixing steps first."},
    {"Specialty", "Ciabatta", 50, 45, true, 1,
     {
         {"Bulk", "handling", "Letter Fold",
          "Fold the dough like a letter, return it under the monitor, then press Complete Step.",
          25, "Letter Fold", "Your dough is ready for its letter fold."},
         {},
         {},
         {},
     },
     "https://www.kingarthurbaking.com/recipes/ciabatta-rolls-recipe",
     nullptr,
     "Complete the recipe's overnight preferment and mixing steps first."},
    {"Specialty", "Soft Pretzels", 50, 0, false, 0, {},
     "https://www.kingarthurbaking.com/recipes/classic-pretzels-recipe"},
    {"Rolls", "Dinner Rolls", 90, 50, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/soft-dinner-rolls-recipe"},
    {"Rolls", "Hawaiian Rolls", 50, 50, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/hawaiian-rolls-recipe",
     nullptr,
     "Complete the recipe's sponge rest and mixing steps first."},
    {"Rolls", "Hamburger Buns", 75, 70, true, 2,
     {
         {"Bulk", "handling", "Degas Fold",
          "Fold the dough over several times to degas it, return it under the monitor, then press Complete Step.",
          35, "Degas Fold", "Your dough is ready for its first degas fold."},
         {"Bulk", "handling", "Degas Fold",
          "Degas the dough again, return it under the monitor, then press Complete Step.",
          65, "Degas Fold", "Your dough is ready for its second degas fold."},
         {},
         {},
     },
     "https://www.kingarthurbaking.com/recipes/sourdough-hamburger-buns-recipe"},
    {"Rolls", "Hot Dog Buns", 90, 90, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/hamburger-or-hot-dog-buns-recipe"},
    {"Rolls", "Hoagie Rolls", 55, 80, true, 1,
     {
         {"Bulk", "handling", "Degas Fold",
          "Turn the dough over and gently deflate it, return it under the monitor, then press Complete Step.",
          25, "Degas Fold", "Your dough is ready to be turned and degassed."},
         {},
         {},
         {},
     },
     "https://www.kingarthurbaking.com/recipes/italian-sub-rolls-recipe",
     nullptr,
     "Complete the recipe's overnight preferment and mixing steps first."},
    {"Rolls", "Kaiser Rolls", 75, 90, true, 0, {},
     "https://www.kingarthurbaking.com/recipes/kaiser-rolls-recipe"},
};

const RecipePreset kCalibrationTestPreset = {"Developer", "Calibration Test", 75, 0, false, 0, {}};
}  // namespace

const RecipePreset* allVisibleRecipePresets(size_t& count) {
  count = sizeof(kVisibleRecipePresets) / sizeof(kVisibleRecipePresets[0]);
  return kVisibleRecipePresets;
}

const RecipePreset& calibrationTestPreset() {
  return kCalibrationTestPreset;
}

String normalizeProfileName(const String& name) {
  if (name == "No-Knead Bread") return "No-Knead Sourdough Bread";
  if (name == "No-Knead Sourdough Bread") return "No-Knead Sourdough Bread";
  if (name == "Baguettes") return "Baguettes";
  if (name == "Neapolitan Pizza") return "Neapolitan Pizza";
  if (name == "Detroit Pizza") return "Detroit Pizza";
  if (name == "Babka") return "Babka";
  if (name == "Rustic Hearth Bread") return "Rustic Sourdough";
  if (name == "Cinnamon Raisin Bread") return "Sandwich Bread";
  if (name == "Pretzels") return "Soft Pretzels";
  if (name == "Calibration Test") return "Calibration Test";
  return name;
}

bool findRecipePreset(const String& name, RecipePreset& preset) {
  const String normalizedName = normalizeProfileName(name);
  size_t count = 0;
  const RecipePreset* presets = allVisibleRecipePresets(count);

  for (size_t index = 0; index < count; ++index) {
    if (normalizedName == presets[index].name) {
      preset = presets[index];
      return true;
    }
  }

  if (normalizedName == kCalibrationTestPreset.name) {
    preset = kCalibrationTestPreset;
    return true;
  }

  return false;
}

String proofProfilesJson() {
  size_t count = 0;
  const RecipePreset* presets = allVisibleRecipePresets(count);

  String json = "[";

  for (size_t index = 0; index < count; ++index) {
    if (index > 0) json += ",";

    const RecipePreset& preset = presets[index];
    const float bulkTarget = adjustedTarget(preset.name, "Bulk", preset.bulk);
    const float finalTarget = preset.hasFinal ? adjustedTarget(preset.name, "Final", preset.final) : 0.0f;
    const String sourceUrl = preset.sourceUrl != nullptr ? String(preset.sourceUrl) : String("");

    json += "{";
    json += "\"category\":\"" + jsonEscape(preset.category) + "\",";
    json += "\"name\":\"" + jsonEscape(preset.name) + "\",";
    json += "\"bulk\":" + String(bulkTarget, 0) + ",";
    json += "\"final\":" + String(finalTarget, 0) + ",";
    json += "\"hasFinal\":" + String(preset.hasFinal ? "true" : "false") + ",";
    json += "\"hasUntrackedFinal\":" + String(preset.untrackedFinalNote != nullptr ? "true" : "false") + ",";
    json += "\"sourceUrl\":\"" + jsonEscape(sourceUrl) + "\"";
    json += "}";
  }

  json += "]";
  return json;
}

bool profileRequiresStageSetupReset(const String& name) {
  const String normalizedName = normalizeProfileName(name);

  return normalizedName == "Country Loaf" ||
         normalizedName == "Sandwich Bread" ||
         normalizedName == "Whole Wheat Sandwich Bread" ||
         normalizedName == "French Bread" ||
         normalizedName == "Sourdough Sandwich Bread" ||
         normalizedName == "Rustic Sourdough" ||
         normalizedName == "No-Knead Sourdough Bread" ||
         normalizedName == "Milk Bread" ||
         normalizedName == "Baguettes" ||
         normalizedName == "Detroit Pizza" ||
         normalizedName == "Focaccia" ||
         normalizedName == "English Muffins" ||
         normalizedName == "Bagels" ||
         normalizedName == "Sourdough Bagels" ||
         normalizedName == "Cinnamon Rolls" ||
         normalizedName == "Sticky Buns" ||
         normalizedName == "Hawaiian Rolls" ||
         normalizedName == "Babka" ||
         normalizedName == "Ciabatta" ||
         normalizedName == "Dinner Rolls" ||
         normalizedName == "Hamburger Buns" ||
         normalizedName == "Hot Dog Buns" ||
         normalizedName == "Hoagie Rolls" ||
         normalizedName == "Kaiser Rolls";
}
