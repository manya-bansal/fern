#ifndef HALIDE_EXAMPLES_H
#define HALIDE_EXAMPLES_H

#include "dependency_lang/data-structure-interface.h"
#include "dependency_lang/dep_lang.h"
#include "dependency_lang/dep_lang_nodes.h"
#include "examples/common-ds.h"
#include "utils/name.h"

namespace examples {

class image : public fern::AbstractDataStructure {
public:
  image(const std::string &name) : name(name) {}

  image() : image(fern::util::uniqueName("weight")) {}

  std::string getTypeName() const override { return "image<float>"; }

  std::string getDataQueryInterface() const override { return {"get_tile"}; }

  std::vector<std::string> getMetaData() const override {
    return {"x", "y", "x_tile_size", "y_tile_size"};
  }

  std::string getDataInsertInterface() const override {
    return {"insert_tile"};
  }

  std::string getAllocData() const override { return {"alloc_image"}; }

  std::string getVarName() const override { return name; }

  std::string getAllocFreeInterface() const override { return "destroy"; }

  bool isTranslationInvariant(int index) const override {
    if (index < 2) {
      return true;
    }
    return false;
  }

private:
  std::string name;
};

class blur_x : public fern::AbstractFunctionCall {
public:
  blur_x() : img(image("img")), result(image("result")) {}

  std::string getName() const override { return "naive_blur_x_take_2_vec"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_input("in", &img);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile, y_tile + 2)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&img))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result))),
    };
  }

  image img;
  image result;
};

class blur_y : public fern::AbstractFunctionCall {
public:
  blur_y() : img(image("img")), result(image("result")) {}

  std::string getName() const override { return "naive_blur_y_take_2_vec"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_input("in", &img);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile + 2, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&img))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result))),
    };
  }

  image img;
  image result;
};

class addition : public fern::AbstractFunctionCall {
public:
  addition() : img(image("img")), result(image("result")) {}

  std::string getName() const override { return "addition"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_input("in", &img);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&img))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result))),
    };
  }

  image img;
  image result;
};

class gray : public fern::AbstractFunctionCall {
public:
  gray() : img(image("img")), result(image("result")) {}

  std::string getName() const override { return "gray_mine"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_input("in", &img);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer(
            {block_input(x, y * 3, x_tile + 6, y_tile * 3 + 6 * 3)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&img))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result))),
    };
  }

  image img;
  image result;
};

class blur_x_unsharp : public fern::AbstractFunctionCall {
public:
  blur_x_unsharp() : img(image("img")), result(image("result")) {}

  std::string getName() const override { return "naive_blur_x_unsharp_vec"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_input("in", &img);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile, y_tile + 6)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&img))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result))),
    };
  }

  image img;
  image result;
};

class blur_y_unsharp : public fern::AbstractFunctionCall {
public:
  blur_y_unsharp() : img(image("img")), result(image("result")) {}

  std::string getName() const override { return "naive_blur_y_unsharp_vec"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_input("in", &img);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_input(x, y, x_tile + 6, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&img))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result))),
    };
  }

  image img;
  image result;
};

class sharpen : public fern::AbstractFunctionCall {
public:
  sharpen()
      : gray(image("gray")), blur_x(image("blur_x")), result(image("result")) {}

  std::string getName() const override { return "sharpen_vec"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_gray("in_gray", &gray);
    fern::DataStructure block_blurx("in_blur", &blur_x);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_gray(x, y, x_tile, y_tile),
                        block_blurx(x, y, x_tile, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&gray))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&blur_x))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result))),
    };
  }

  image gray;
  image blur_x;
  image result;
};

class ratio : public fern::AbstractFunctionCall {
public:
  ratio()
      : gray(image("gray")), sharpen_im(image("sharpen_im")),
        result(image("result")) {}

  std::string getName() const override { return "ratio_vec"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_gray("in_gray", &gray);
    fern::DataStructure block_blurx("in_blur", &sharpen_im);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_gray(x, y, x_tile, y_tile),
                        block_blurx(x, y, x_tile, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&gray))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&sharpen_im))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result))),
    };
  }

  image gray;
  image sharpen_im;
  image result;
};

class output_assign : public fern::AbstractFunctionCall {
public:
  output_assign()
      : img(image("img")), ratio_im(image("ratio_im")),
        result(image("result")) {}

  std::string getName() const override { return "output_assign_mine"; }

  fern::DependencySubset getDataRelationship() const override {
    fern::Variable x("x", false, false, true);
    fern::Variable y("y", false, false, true);
    fern::DataStructure block_out("out", &result);
    fern::DataStructure block_img("block_img", &img);
    fern::DataStructure block_ratio("block_ratio", &ratio_im);

    fern::Variable x_tile("x_tile", false, false, false);
    fern::Interval x_loop(x, 0, result["logical_rows"], x_tile);

    fern::Variable y_tile("y_tile", false, false, false);
    fern::Interval y_loop(y, 0, result["logical_cols"], y_tile);

    return x_loop(y_loop(fern::ComputationAnnotation(
        fern::Producer(block_out(x, y, x_tile, y_tile)),
        fern::Consumer({block_img(x, y * 3, x_tile, y_tile * 3),
                        block_ratio(x, y, x_tile, y_tile)}))));
  }

  std::vector<fern::Argument> getArguments() override {
    return {
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&img))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&ratio_im))),
        fern::Argument(std::make_shared<fern::DataStructureArg>(fern::DataStructurePtr(&result))),
    };
  }

  image img;
  image ratio_im;
  image result;
};

} // namespace examples

#endif