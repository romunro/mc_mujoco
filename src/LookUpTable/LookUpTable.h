/*
 * Copyright (c) 2020.
 * All rights reserved. This program is made available under the terms of the
 * Eclipse Public License v1.0 which accompanies this distribution, and is
 * available at http://www.eclipse.org/legal/epl-v10.html
 * Contributors:
 * AIST-CNRS JRL (Joint Robotics Laboratory), UMI3218/IRL
 */

/*! @file
 *  @brief Implementation of a Look-Up Table (LUT)
 *  @author Rafael Cisneros, Arnaud Tanguy
 *
 *  This implementation is based on the one developed by Arnaud Tanguy:
 *  https://github.com/arntanguy/mc_rtc/blob/topic/hmc/include/mc_planning/LookupTable.h
 */

#ifndef __LOOKUPTABLE_H__
#define __LOOKUPTABLE_H__

#include <cmath>
#include <stdexcept>
#include <utility>
#include <vector>

#include <iostream>

namespace utl
{

/**
 * @brief Stores precomputed values of a function for fast look-up
 * @tparam T Data type for the function arguments and result value
 *
 * This class provides a Look-Up Table (LUT) implementation intended for fast evaluation
 * of computationally expensive functions. This is done by pre-generating a table
 * of possible function results in a given resolution, and computing a linear
 * interpolation of the pre-recorded data to retrieve an approximation of the
 * function evaluation for any data within the domain boundaries.
 *
 * It is possible to determine the behavior of the LUT outside of the
 * domain boundaries, for it to retrieve
 *   a) Fail - An error
 *   b) BoundValue - The value at the boundary (min or max)
 *   c) Zero - Just zero
 *
 * On construction, the provided function `f` is evaluated for values of `x`
 * linearly distributed between `min` and `max` by using `step`.
 * Function values \f$ f(x) \f$ are retrieved by calling the operator ()(const T & value)
 *
 * @warning \f$ f(min) \$ is always included in the LUT,
 *          but \f$ f(max) \$ may not, unless it can be reached
 *          with an integer number of steps.
 */

template<typename T>
class LookUpTable
{
public:
  enum OutOfBounds
  {
    Fail,
    BoundValue,
    Zero
  };

  /**
   * @brief Default constructor that does not create a table
   *        By default it will retrieve zero when evaluation a value outside of the
   *        domain boundaries
   */
  LookUpTable()
  {
    outbounds_ = Zero;
  }

  /**
   * @brief Constructor that includes the creation of the LUT and allows to specify
   *        the behavior outside of the domain boundaries
   *
   * @param outbounds Specification of the behavior outside of the boundaries
   *        Options are Fail, BoundValue or Zero
   *
   * @see create(const T & min, const T & max, const T & step, MappingFunction f)
   */
  template<typename MappingFunction>
  LookUpTable(const T & min, const T & max, const T & step, MappingFunction f, OutOfBounds outbounds = Zero)
  {
    outbounds_ = outbounds;
    create(min, max, step, f);
  }

  /**
   * @brief Evaluate and store given function results
   *
   * @param min Minimum argument value
   * @param max Maximum argument value
   * @param step Step size to compute intermediate points
   * @param f Function to evaluate (it has to be a lambda function)
   *
   */
  template<typename MappingFunction>
  bool create(const T & min, const T & max, const T & step, MappingFunction f)
  {
    if(min > max || step == 0) return false;

    min_ = min;
    max_ = max;
    step_ = step;

    size_t tableSize = floor((max_ - min_) / step_) + 1;
    table_.resize(tableSize);

    for(size_t i = 0; i < table_.size(); ++i)
    {
      table_[i].first = min_ + static_cast<T>(i) * step_;
      table_[i].second = f(table_[i].first);
    }

    return true;
  }

  /**
   * @brief Allows to check if the table is empty or not
   * @return true if the table is empty, false if not
   */
  bool empty()
  {
    return table_.empty();
  }

  /**
   * @brief Retrieves the computed value \f$ f(x) \f$
   *
   * @throws std::out_of_range
   *         If outbounds=Fail and x is out of bounds (x < min or x > max)
   *
   * @param x Argument of the function `f`
   *
   * @return If x > min and x < max the result is computed by using linear interpolation
   *                                of the pre-computed values contained in the table
   *         Else it behaves as dictated by outbounds
   */
  T operator()(const T & x)
  {
    if(table_.empty()) throw std::out_of_range("Uninitialized table. Call create() before use.");

    size_t maxIndex = table_.size() - 1;

    T val;

    if(x < min_ || x > max_)
    {

      switch(outbounds_)
      {

        case Zero:
          val = static_cast<T>(0);
          break;

        case BoundValue:
          if(x < min_)
            val = table_[0].second;
          else
            val = table_[maxIndex].second;
          break;

        default:
          throw std::out_of_range("Out of bound access");
          break;
      }
    }
    else
    {

      size_t i = std::min(static_cast<size_t>(floor((x - min_) / step_)), maxIndex);

      if(i < maxIndex)
      {
        val = table_[i].second
              + (table_[i + 1].second - table_[i].second) * (x - table_[i].first)
                    / (table_[i + 1].first - table_[i].first);
      }
      else
        val = table_[i].second;
    }

    return val;
  }

protected:
  std::vector<std::pair<T, T>> table_;
  T min_, max_, step_;
  OutOfBounds outbounds_;
};

} // namespace utl

#endif // __LOOKUPTABLE_H__
