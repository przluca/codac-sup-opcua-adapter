/******************************************************************************
* $HeadURL: https://svnpub.iter.org/codac/iter/codac/dev/units/m-cpp-common/tags/CODAC-CORE-6.2B2/src/main/c++/base/Statistics.h $
* $Id: Statistics.h 100703 2019-07-11 15:23:56Z bauvirb $
*
* Project       : CODAC Core System
*
* Description   : Statistical computation class definition
*
* Author        : Bertrand Bauvir (IO)
*
* Copyright (c) : 2010-2019 ITER Organization,
*                 CS 90 046
*                 13067 St. Paul-lez-Durance Cedex
*                 France
*
* This file is part of ITER CODAC software.
* For the terms and conditions of redistribution or use of this software
* refer to the file ITER-LICENSE.TXT located in the top level directory
* of the distribution package.
******************************************************************************/

/**
 * @file Statistics.h
 * @brief Header file for Statistics<Type> class
 * @date 23/03/2017
 * @author Bertrand Bauvir (IO)
 * @copyright 2010-2017 ITER Organization
 * @detail This header file contains the definition of the Statistics<Type> class.
 */

#ifndef _Statistics_h_
#define _Statistics_h_

// Global header files

#include <math.h>
#include <limits>
#include <new> // std::nothrow

// Local header files

#include "BasicTypes.h"
#include "CircularBuffer.h"
#include "SquareRoot.h"

// Constants

#define DEFAULT_WINDOW_SIZE 1024

// Type definition

namespace ccs {

namespace base {

/**
 * @brief The class provides templated statistics computation.
 * @detail The class allocates circular buffers to store values of samples and
 * computes average, standard deviation, minimum and maximum over a moving time
 * window. As such, the sum of samples and sum of squares is computed upon calling
 * the PushSample() method in a way to minimize operations. The minimum or maximum
 * over the historical buffer are re-computed only when needed, i.e. in case the
 * oldest sample being pushed out of the moving time window is equal to the stored
 * minimum or maximum.
 * The implementation does not perform division, rather uses bit shift operation
 * for integer types, and pre-computes 1.0 / size for floating point types. As such,
 * the computation of average and standard deviation is only exact after the window
 * has been fully populated once with samples.
 * Consequent to that above, the window size for integer types is limited to power
 * of 2, e.g. 64, 1024, etc. In case a different window size is specified, the 
 * highest power of 2 smaller than the specified size is being considered. No
 * limitation for floating point types.
 */

template <typename Type> class Statistics {

  private:

    types::uint32 m_size;    // Window size
    types::uint32 m_counter; // Number of samples inserted

    Type m_Xavg;
    Type m_Xdiv; 
    Type m_Xmax; 
    Type m_Xmin;
    Type m_Xrms;
    Type m_Xspl;
    Type m_Xstd; 

    base::CircularBuffer<Type>* m_Xwin; // Sample buffer
    base::CircularBuffer<Type>* m_Xsq;  // Sample buffer (squared)

    /**
     * @brief Average of squared samples over the moving window.
     * @return average of sample squares.
     */

    Type GetRmsSq (void) const;

    /**
     * @brief Finds the maximum of samples in the sample buffer.
     * @return maximum.
     */

    Type FindMax (void) const;

    /**
     * @brief Finds the minimum of samples in the sample buffer.
     * @return minimum.
     */

    Type FindMin (void) const;

  public:

    /**
     * @brief Constructor. 
     * @detail Allocates memory buffers to store samples, and squares of, in a moving
     * time window. In case of integer types, the actual window size will be the highest
     * power of 2 smaller or equal to the specified size.
     */

    Statistics (const types::uint32 size);

    /**
     * @brief Destructor. Frees allocated memory buffers.
     */

    virtual ~Statistics (void); 

    /**
     * @brief Initialiser. Clears out sample buffer and initialises accumulators.
     * @return TRUE if buffers successfully cleared.
     */

    bool Reset (void);

    /**
     * @brief Accessor. Inserts new sample in the moving time window.
     * @detail The method inserts the sample in the historical buffer and re-computes the
     * sum of samples over the time window with two operations, i.e. to remove the oldest
     * sample leaving the buffer from the cumulative sum before adding the new one.
     * The minimum and maximum sample attributes are changed if the newly inserted sample
     * is respectively smaller or higher than the currently stored minimum or maximum. The
     * historycal buffer is fully parsed in case the oldest sample leaving the buffer is
     * equal to the recorded minimum or maximum. 
     * The new sample is also squared and inserted into a second buffer, with similar sum
     * management.
     * The average, root mean square, and standard deviation are only computed when the 
     * corresponding accessors are being called.
     * @return TRUE if buffer properly allocated.
     */

    bool PushSample (Type sample);

    /**
     * @brief Accessor. Returns the window size.
     * @return actual window size.
     */

    types::uint32 GetSize (void) const;

    /**
     * @brief Accessor. Returns the number of samples inserted in the buffer.
     * @return number of samples.
     */

    types::uint32 GetCounter (void) const;

    /**
     * @brief Accessor. Retrieves the last inserted sample.
     * @return Last inserted sample.
     */

    Type GetSample (void) const;

    /**
     * @brief Average over the moving window of samples.
     * @detail The average is computed  in this method from the cumulative sum of samples
     * over the time window maintained/updated by the PushSample() method. The average is
     * computed by bit-shifting the accumulator in case of integer types or by multiplying
     * it with pre-computed (1.0 / size) for floating point types.
     * @return average.
     */

    Type GetAvg (void) const;

    /**
     * @brief Maximum over the moving window of samples. No computation involved.
     * @return maximum.
     */

    Type GetMax (void) const;

    /**
     * @brief Minimum over the moving window of samples. No computation involved.
     * @return minimum.
     */

    Type GetMin (void) const;

    /**
     * @brief Root mean squares over the moving window of samples.
     * @detail The root mean square is computed in this method from the cumulative sum of
     * squared samples over the time window maintained/updated by the PushSample() method.
     * The rms is computed by bit-shifting the accumulator in case of integer types or by
     * multiplying it with pre-computed (1.0 / size) for floating point types. And
     * eventually calling SquareRoot() of the result.
     * @return rms.
     */

    Type GetRms (void) const;

    /**
     * @brief Standard deviation over the moving window of samples.
     * @detail This is the most computation heavy accessor since it involves computing the
     * average, squaring it and substrating it from the average of the squared samples over
     * the time window.
     * @return standard deviation.
     */

    Type GetStd (void) const;

    /**
     * @brief Sum of all samples over the moving window of samples.
     * @return sum.
     */

    Type GetSum (void) const;

};

// Global variables

// Function declaration

// Function definition

template <typename Type> bool Statistics<Type>::Reset (void) 
{

  // Reset attributes
  m_counter = 0u;

  m_Xspl = 0;
  m_Xavg = 0;
  m_Xmax = std::numeric_limits<Type>::min();
  m_Xmin = std::numeric_limits<Type>::max();
  m_Xrms = 0;
  m_Xstd = 0;
  
  // Reset sample buffers
  m_Xwin->Initialise(m_Xspl); 
  m_Xsq->Initialise(m_Xspl);
  
  return true;
  
}

template <> inline bool Statistics<types::float32>::Reset (void) 
{

  using namespace types; 

  // Reset attributes
  m_counter = 0u;
  
  m_Xspl = 0.0F;
  m_Xavg = 0.0F;
  m_Xmax = -1.0F * std::numeric_limits<float32>::max();
  m_Xmin = std::numeric_limits<float32>::max();
  m_Xrms = 0.0F;
  m_Xstd = 0.0F;
  
  // Reset sample buffers
  m_Xwin->Initialise(m_Xspl); 
  m_Xsq->Initialise(m_Xspl);
  
  return true;
  
}

template <> inline bool Statistics<types::float64>::Reset (void) 
{ 

  using namespace types; 

  // Reset attributes
  m_counter = 0u;
  
  m_Xspl = 0.0;
  m_Xavg = 0.0;
  m_Xmax = -1.0 * std::numeric_limits<float64>::max();
  m_Xmin = std::numeric_limits<float64>::max();
  m_Xrms = 0.0;
  m_Xstd = 0.0;
  
  // Reset sample buffers
  m_Xwin->Initialise(m_Xspl); 
  m_Xsq->Initialise(m_Xspl);
  
  return true;
  
}

template <typename Type> Statistics<Type>::Statistics (const types::uint32 size)
{

  using namespace types; 

  m_size = 1u; 
  m_Xdiv = 0;
  
  uint32 tmp = size;
  
  while (tmp > 1u) 
    { 
      tmp >>= 1u; 
      m_size <<= 1u;
      m_Xdiv += 1;
    }
  
  // Instantiate sample buffers
  m_Xwin = new (std::nothrow) CircularBuffer<Type> (m_size); 
  m_Xsq  = new (std::nothrow) CircularBuffer<Type> (m_size); 
  
  (void)Reset();
  
}

template <> inline Statistics<types::float32>::Statistics (const types::uint32 size)
{

  using namespace types; 

  m_size = size;
  m_Xdiv = 1.0F / static_cast<float32>(m_size);
  
  // Instantiate sample buffers
  m_Xwin = new (std::nothrow) CircularBuffer<float32> (m_size); 
  m_Xsq  = new (std::nothrow) CircularBuffer<float32> (m_size); 
  
  (void)Reset();
  
}

template <> inline Statistics<types::float64>::Statistics (const types::uint32 size)
{

  using namespace types; 

  m_size = size;
  m_Xdiv = 1.0 / static_cast<float64>(m_size);
  
  // Instantiate sample buffers
  m_Xwin = new (std::nothrow) CircularBuffer<float64> (m_size); 
  m_Xsq  = new (std::nothrow) CircularBuffer<float64> (m_size); 
  
  (void)Reset();
  
}

template <typename Type> Statistics<Type>::~Statistics (void)
{

  if (m_Xwin != static_cast<CircularBuffer<Type> *>(NULL)) 
    { 
      delete m_Xwin; 
      m_Xwin = static_cast<CircularBuffer<Type> *>(NULL); 
    }
  
  if (m_Xsq != static_cast<CircularBuffer<Type> *>(NULL)) 
    {
      delete m_Xsq;
      m_Xsq = static_cast<CircularBuffer<Type> *>(NULL);
    }
  
}

template <typename Type> Type Statistics<Type>::FindMax (void) const 
{

  using namespace types; 

  Type max = std::numeric_limits<Type>::min();
  
  for (uint32 index = 0u; index < m_size; index++) 
    {
    
      Type sample = static_cast<Type>(0);
      
      m_Xwin->GetData(sample, index);
      
      if (sample > max) 
        {
          max = sample;
        }

    }

  return max;

}

template <> inline types::float32 Statistics<types::float32>::FindMax (void) const 
{

  using namespace types; 

  float32 max = -1.0F * std::numeric_limits<float32>::max();
  
  for (uint32 index = 0u; index < m_size; index++) 
    {

      float32 sample = 0.0F;
      
      m_Xwin->GetData(sample, index);
      
      if (sample > max) 
        {
          max = sample;
        }

    }
  
  return max;

}

template <> inline types::float64 Statistics<types::float64>::FindMax (void) const 
{

  using namespace types; 

  float64 max = -1.0 * std::numeric_limits<float64>::max();
  
  for (uint32 index = 0u; index < m_size; index++) 
    {

      float64 sample = 0.0;
      
      m_Xwin->GetData(sample, index);
      
      if (sample > max) 
        {
          max = sample;
        }

    }
  
  return max;

}

template <typename Type> Type Statistics<Type>::FindMin (void) const 
{

  using namespace types; 

  Type min = std::numeric_limits<Type>::max();
  
  for (uint32 index = 0u; index < m_size; index++) 
    {

      Type sample = static_cast<Type>(0);;
      
      m_Xwin->GetData(sample, index);
      
      if (sample < min) 
        {
          min = sample;
        }

    }

  return min;

}

template <typename Type> bool Statistics<Type>::PushSample (Type sample) 
{

  m_Xspl = sample;

  if (m_counter < m_size) 
    {
      m_counter++;
    }

  Type oldest;

  // Update sample buffer
  bool status = m_Xwin->PushData(m_Xspl, oldest);

  if (status) 
    {
      // Compute average
      m_Xavg -= oldest; // Remove oldest sample from the accumulator
      m_Xavg += m_Xspl; // Sum of all sample in time window
      
      // Update max/min, if necessary
      if (m_Xspl > m_Xmax) 
        {
          m_Xmax = m_Xspl; 
        } 
      else if (oldest == m_Xmax) // The removed sample was the max over the time window
        { 
          m_Xmax = FindMax();
        }
      
      if (m_Xspl < m_Xmin) 
        {
          m_Xmin = m_Xspl; 
        } 
      else if (oldest == m_Xmin) // The removed sample was the min over the time window
        { 
          m_Xmin = FindMin();
        }
    }

  Type Xspl_sq  = m_Xspl * m_Xspl; // Square of the sample

  // Update sample buffer
  status = m_Xsq->PushData(Xspl_sq, oldest);
  
  if (status) 
    {
      // Compute root mean square
      m_Xrms -= oldest; // Remove oldest sample from the accumulator
      m_Xrms += Xspl_sq;    // Sum of squares of all samples in time window
    }
  
  return status;

}

template <typename Type> Type Statistics<Type>::GetAvg (void) const { Type avg = m_Xavg >> m_Xdiv; return avg; }

template <> inline types::float32 Statistics<types::float32>::GetAvg (void) const { using namespace types; float32 avg = m_Xavg * m_Xdiv; return avg; }
template <> inline types::float64 Statistics<types::float64>::GetAvg (void) const { using namespace types; float64 avg = m_Xavg * m_Xdiv; return avg; }

template <typename Type> Type Statistics<Type>::GetRmsSq (void) const { Type rms_sq = m_Xrms >> m_Xdiv; return rms_sq; }

template <> inline types::float32 Statistics<types::float32>::GetRmsSq (void) const { using namespace types; float32 rms_sq = m_Xrms * m_Xdiv; return rms_sq; }
template <> inline types::float64 Statistics<types::float64>::GetRmsSq (void) const { using namespace types; float64 rms_sq = m_Xrms * m_Xdiv; return rms_sq; }

template <typename Type> Type Statistics<Type>::GetRms (void) const 
{

  Type rms_sq = GetRmsSq();
  Type rms = SquareRoot<Type>(rms_sq);

  return rms;

}

template <typename Type> Type Statistics<Type>::GetStd (void) const 
{

  Type avg = GetAvg();
  Type avg_sq = avg * avg;
  Type rms_sq = GetRmsSq();
  Type std = SquareRoot<Type>(rms_sq - avg_sq);
  
  return std;
  
}

template <typename Type> types::uint32 Statistics<Type>::GetSize (void) const { return m_size; }
template <typename Type> types::uint32 Statistics<Type>::GetCounter (void) const { return m_counter; }
template <typename Type> Type Statistics<Type>::GetSample (void) const { return m_Xspl; }
template <typename Type> Type Statistics<Type>::GetSum (void) const { return m_Xavg; }
template <typename Type> Type Statistics<Type>::GetMax (void) const { return m_Xmax; }
template <typename Type> Type Statistics<Type>::GetMin (void) const { return m_Xmin; }

} // namespace base

} // namespace ccs

#endif // _Statistics_h_


