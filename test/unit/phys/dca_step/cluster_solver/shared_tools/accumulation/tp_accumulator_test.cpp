// Copyright (C) 2009-2016 ETH Zurich
// Copyright (C) 2007?-2016 Center for Nanophase Materials Sciences, ORNL
// All rights reserved.
//
// See LICENSE.txt for terms of usage.
// See CITATION.txt for citation guidelines if you use this code for scientific publications.
//
// Author: Giovanni Balduzzi (gbalduzz@itp.phys.ethz.ch)
//
// This file implements a no-change test for the two point accumulation of a mock configuration.

#include "dca/phys/dca_step/cluster_solver/ctaux/accumulator/tp/accumulator_nonlocal_chi.hpp"
#include "dca/phys/dca_step/cluster_solver/ctaux/accumulator/tp/accumulator_nonlocal_g.hpp"

#include <array>
#include "gtest/gtest.h"
#include <string>
#include <vector>

#include "dca/function/function_utils.hpp"
#include "dca/math/random/std_random_wrapper.hpp"
#include "test/unit/phys/dca_step/cluster_solver/test_setup.hpp"

constexpr bool UPDATE_RESULTS = false;

struct ConfigElement {
  double get_tau() const {
    return tau_;
  }
  double get_band() const {
    return band_;
  }
  double get_r_site() const {
    return r_;
  }

  int band_;
  int r_;
  double tau_;
};
using Configuration = std::array<std::vector<ConfigElement>, 2>;
using MatrixPair = std::array<dca::linalg::Matrix<double, dca::linalg::CPU>, 2>;

using G0Setup = typename dca::testing::G0Setup<dca::testing::LatticeBilayer>;
const std::string input_dir =
    DCA_SOURCE_DIR "/test/unit/phys/dca_step/cluster_solver/shared_tools/accumulation/";

void prepareRandomConfig(Configuration& config, MatrixPair& M, std::array<int, 2> n);
std::string toString(dca::phys::FourPointType);

TEST_F(G0Setup, Accumulate) {
  const std::array<int, 2> n{18, 22};
  MatrixPair M;
  Configuration config;
  prepareRandomConfig(config, M, n);

  auto& parameters = G0Setup::parameters;
  auto& data = *G0Setup::data;
  auto& G4 = data.get_G4_k_k_w_w();
  auto G4_check(G4);

  dca::phys::solver::ctaux::accumulator_nonlocal_G<G0Setup::Parameters, G0Setup::Data> nonlocal_G_obj(
      parameters, data, 0);
  dca::phys::solver::ctaux::accumulator_nonlocal_chi<G0Setup::Parameters, G0Setup::Data> nonlocal_chi_obj(
      parameters, data, 0, G4);

  dca::io::HDF5Writer writer;
  dca::io::HDF5Reader reader;

  if(UPDATE_RESULTS)
    writer.open_file("G4_result.hdf5");
  else
    reader.open_file(input_dir + "G4_result.hdf5");

  for (const dca::phys::FourPointType type :
       {dca::phys::PARTICLE_HOLE_TRANSVERSE, dca::phys::PARTICLE_HOLE_MAGNETIC,
        dca::phys::PARTICLE_HOLE_CHARGE, dca::phys::PARTICLE_PARTICLE_UP_DOWN}) {
    G4 = 0;
    parameters.set_four_point_type(type);

    nonlocal_G_obj.execute(config[0], M[0], config[1], M[1]);
    const int sign = 1;
    nonlocal_chi_obj.execute(sign, nonlocal_G_obj);

    if(UPDATE_RESULTS) {
      G4.set_name("G4_" + toString(type));
      writer.execute(G4);
    }
    else{
      G4_check.set_name("G4_" + toString(type));
      reader.execute(G4_check);
      const auto diff = dca::func::utils::difference(G4, G4_check);
      EXPECT_GT(1e-8, diff.l_inf);
    }
  }

  if(UPDATE_RESULTS)
    writer.close_file();
  else
    reader.close_file();
}

void prepareRandomConfig(Configuration& config, MatrixPair& M, const std::array<int, 2> ns) {
  dca::math::random::StdRandomWrapper<std::ranlux48_base> rng(0, 1, 0);

  for (int s = 0; s < 2; ++s) {
    const int n = ns[s];
    config[s].resize(n);
    M[s].resize(n);
    for (int i = 0; i < n; ++i) {
      const double tau = rng() - 0.5;
      const int r = rng() * G0Setup::RDmn::dmn_size();
      const int b = rng() * G0Setup::BDmn::dmn_size();
      config[s][i] = ConfigElement{b, r, tau};
    }

    for (int j = 0; j < n; ++j)
      for (int i = 0; i < n; ++i)
        M[s](i, j) = 2 * rng() - 1.;
  }
}

std::string toString(dca::phys::FourPointType type) {
  switch (type) {
    case dca::phys::NONE:
      return "none";

   case dca::phys::PARTICLE_PARTICLE_UP_DOWN:
     return "pp_up_down";

   case dca::phys::PARTICLE_HOLE_TRANSVERSE:
     return "ph_transverse";

   case dca::phys::PARTICLE_HOLE_MAGNETIC:
     return "ph_magnetic";

   case dca::phys::PARTICLE_HOLE_CHARGE:
     return "ph_charge";

   default:
     throw std::logic_error("type not valid.");
  }
}