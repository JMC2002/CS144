﻿#include "wrapping_integers.hh"
#include <algorithm>
using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return zero_point + n;
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  uint64_t const dis = raw_value_ - wrap( checkpoint, zero_point ).raw_value_;
  return checkpoint + ( dis <= ( 1u << 31 ) ? dis : min( dis, -dis ) );
}