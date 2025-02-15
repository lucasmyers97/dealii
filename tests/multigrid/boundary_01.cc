// ---------------------------------------------------------------------
//
// Copyright (C) 2006 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------


// check MGTools::make_boundary_list

#include <deal.II/base/function.h>

#include <deal.II/dofs/dof_accessor.h>

#include <deal.II/fe/fe_dgq.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_raviart_thomas.h>
#include <deal.II/fe/fe_system.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include <deal.II/lac/block_vector.h>
#include <deal.II/lac/vector.h>

#include <deal.II/multigrid/mg_tools.h>

#include <algorithm>

#include "../tests.h"



void
log_vector(const std::vector<std::set<types::global_dof_index>> &count)
{
  for (unsigned int l = 0; l < count.size(); ++l)
    {
      deallog << "Level " << l << ':';
      for (std::set<types::global_dof_index>::const_iterator c =
             count[l].begin();
           c != count[l].end();
           ++c)
        deallog << ' ' << *c;
      deallog << std::endl;
    }
}


template <int dim>
void
check_fe(FiniteElement<dim> &fe)
{
  deallog << fe.get_name() << std::endl;

  Triangulation<dim> tr(Triangulation<dim>::limit_level_difference_at_vertices);
  GridGenerator::hyper_cube(tr);
  tr.refine_global(2);
  Functions::ZeroFunction<dim>                        zero;
  std::map<types::boundary_id, const Function<dim> *> fmap;
  fmap.insert(std::make_pair(0, &zero));

  DoFHandler<dim> mgdof(tr);
  mgdof.distribute_dofs(fe);
  mgdof.distribute_mg_dofs();

  std::vector<std::set<types::global_dof_index>> boundary_indices(
    tr.n_levels());
  MGTools::make_boundary_list(mgdof, fmap, boundary_indices);
  log_vector(boundary_indices);
}


template <int dim>
void
check()
{
  FE_Q<dim> q1(1);
  FE_Q<dim> q2(2);
  //  FE_DGQ<dim> dq1(1);

  FESystem<dim> s1(q1, 2, q2, 1);

  check_fe(q1);
  check_fe(q2);
  check_fe(s1);
}

int
main()
{
  initlog();
  deallog << std::setprecision(3);

  check<2>();
  check<3>();
}
