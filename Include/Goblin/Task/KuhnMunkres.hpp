/* Copyright 2010-2011 Ilkka Halila
This file is part of Goblin Camp.

Goblin Camp is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Goblin Camp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License 
along with Goblin Camp. If not, see <http://www.gnu.org/licenses/>.*/
#pragma once

#include <vector>
#include <boost/numeric/ublas/matrix.hpp>

/**
 * The Hungarian method is a combinatorial optimization algorithm that solves
 * the assignment problem in polynomial time and which anticipated later
 * primal–dual methods. It was developed and published in 1955 by Harold Kuhn,
 * who gave the name "Hungarian method" because the algorithm was largely based
 * on the earlier works of two Hungarian mathematicians:
 * Dénes Kőnig and Jenő Egerváry.
 * <br><br>
 *
 * The assignment problem is a fundamental combinatorial optimization problem.
 * In its most general form, the problem is as follows:
 * <br><br>
 *
 * <em>
 * The problem instance has a number of agents and a number of tasks. Any agent
 * can be assigned to perform any task, incurring some cost that may vary
 * depending on the agent-task assignment. It is required to perform as many
 * tasks as possible by assigning at most one agent to each task and at most
 * one task to each agent, in such a way that the total cost of the assignment
 * is minimized.
 * </em><br><br>
 *
 * <b>
 * Example:
 * </b><br><br>
 *
 * In this simple example there are three workers: Paul, Dave, and Chris. One
 * of them has to clean the bathroom, another sweep the floors and the third
 * washes the windows, but they each demand different pay for the various tasks.
 * The problem is to find the lowest-cost way to assign the jobs. The problem
 * can be represented in a matrix of the costs of the workers doing the jobs.
 * For example:
 * <br><br>
 *
 * Clean bathroom | Sweep floors | Wash windows
 * Paul  | $2 | $3 | $3
 * Dave  | $3 | $2 | $3
 * Chris | $3 | $3 | $2
 *
 * <br><br>
 * The Hungarian method, when applied to the above table, would give the
 * minimum cost: this is $6, achieved by having Paul clean the bathroom, Dave
 * sweep the floors, and Chris wash the windows.
 * <br><br>
 *
 * Ref: https://en.wikipedia.org/wiki/Hungarian_algorithm
 *
 */
std::vector<int> FindBestMatching(boost::numeric::ublas::matrix<int> costs);
