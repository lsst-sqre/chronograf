// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

// Unit tests for DataType (and subclasses), Field, and Schema

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <iostream>

#include <gtest/gtest.h>

#include "arrow/sparse_tensor.h"
#include "arrow/test-util.h"
#include "arrow/type.h"

namespace arrow {

static inline void CheckSparseIndexFormatType(SparseTensorFormat::type expected,
                                              const SparseTensor& sparse_tensor) {
  ASSERT_EQ(expected, sparse_tensor.format_id());
  ASSERT_EQ(expected, sparse_tensor.sparse_index()->format_id());
}

TEST(TestSparseCOOTensor, CreationEmptyTensor) {
  std::vector<int64_t> shape = {2, 3, 4};
  SparseTensorImpl<SparseCOOIndex> st1(int64(), shape);

  std::vector<std::string> dim_names = {"foo", "bar", "baz"};
  SparseTensorImpl<SparseCOOIndex> st2(int64(), shape, dim_names);

  ASSERT_EQ(0, st1.non_zero_length());
  ASSERT_EQ(0, st2.non_zero_length());

  ASSERT_EQ(24, st1.size());
  ASSERT_EQ(24, st2.size());

  ASSERT_EQ("foo", st2.dim_name(0));
  ASSERT_EQ("bar", st2.dim_name(1));
  ASSERT_EQ("baz", st2.dim_name(2));

  ASSERT_EQ("", st1.dim_name(0));
  ASSERT_EQ("", st1.dim_name(1));
  ASSERT_EQ("", st1.dim_name(2));
}

TEST(TestSparseCOOTensor, CreationFromNumericTensor) {
  std::vector<int64_t> shape = {2, 3, 4};
  std::vector<int64_t> values = {1, 0,  2, 0,  0,  3, 0,  4, 5, 0,  6, 0,
                                 0, 11, 0, 12, 13, 0, 14, 0, 0, 15, 0, 16};
  std::shared_ptr<Buffer> buffer = Buffer::Wrap(values);
  std::vector<std::string> dim_names = {"foo", "bar", "baz"};
  NumericTensor<Int64Type> tensor1(buffer, shape);
  NumericTensor<Int64Type> tensor2(buffer, shape, {}, dim_names);
  SparseTensorImpl<SparseCOOIndex> st1(tensor1);
  SparseTensorImpl<SparseCOOIndex> st2(tensor2);

  CheckSparseIndexFormatType(SparseTensorFormat::COO, st1);

  ASSERT_EQ(12, st1.non_zero_length());
  ASSERT_TRUE(st1.is_mutable());

  ASSERT_EQ("foo", st2.dim_name(0));
  ASSERT_EQ("bar", st2.dim_name(1));
  ASSERT_EQ("baz", st2.dim_name(2));

  ASSERT_EQ("", st1.dim_name(0));
  ASSERT_EQ("", st1.dim_name(1));
  ASSERT_EQ("", st1.dim_name(2));

  const int64_t* ptr = reinterpret_cast<const int64_t*>(st1.raw_data());
  for (int i = 0; i < 6; ++i) {
    ASSERT_EQ(i + 1, ptr[i]);
  }
  for (int i = 0; i < 6; ++i) {
    ASSERT_EQ(i + 11, ptr[i + 6]);
  }

  const auto& si = internal::checked_cast<const SparseCOOIndex&>(*st1.sparse_index());
  ASSERT_EQ(std::string("SparseCOOIndex"), si.ToString());

  std::shared_ptr<SparseCOOIndex::CoordsTensor> sidx = si.indices();
  ASSERT_EQ(std::vector<int64_t>({12, 3}), sidx->shape());
  ASSERT_TRUE(sidx->is_column_major());

  // (0, 0, 0) -> 1
  ASSERT_EQ(0, sidx->Value({0, 0}));
  ASSERT_EQ(0, sidx->Value({0, 1}));
  ASSERT_EQ(0, sidx->Value({0, 2}));

  // (0, 0, 2) -> 2
  ASSERT_EQ(0, sidx->Value({1, 0}));
  ASSERT_EQ(0, sidx->Value({1, 1}));
  ASSERT_EQ(2, sidx->Value({1, 2}));

  // (0, 1, 1) -> 3
  ASSERT_EQ(0, sidx->Value({2, 0}));
  ASSERT_EQ(1, sidx->Value({2, 1}));
  ASSERT_EQ(1, sidx->Value({2, 2}));

  // (1, 2, 1) -> 15
  ASSERT_EQ(1, sidx->Value({10, 0}));
  ASSERT_EQ(2, sidx->Value({10, 1}));
  ASSERT_EQ(1, sidx->Value({10, 2}));

  // (1, 2, 3) -> 16
  ASSERT_EQ(1, sidx->Value({11, 0}));
  ASSERT_EQ(2, sidx->Value({11, 1}));
  ASSERT_EQ(3, sidx->Value({11, 2}));
}

TEST(TestSparseCOOTensor, CreationFromTensor) {
  std::vector<int64_t> shape = {2, 3, 4};
  std::vector<int64_t> values = {1, 0,  2, 0,  0,  3, 0,  4, 5, 0,  6, 0,
                                 0, 11, 0, 12, 13, 0, 14, 0, 0, 15, 0, 16};
  std::shared_ptr<Buffer> buffer = Buffer::Wrap(values);
  std::vector<std::string> dim_names = {"foo", "bar", "baz"};
  Tensor tensor1(int64(), buffer, shape);
  Tensor tensor2(int64(), buffer, shape, {}, dim_names);
  SparseTensorImpl<SparseCOOIndex> st1(tensor1);
  SparseTensorImpl<SparseCOOIndex> st2(tensor2);

  ASSERT_EQ(12, st1.non_zero_length());
  ASSERT_TRUE(st1.is_mutable());

  ASSERT_EQ("foo", st2.dim_name(0));
  ASSERT_EQ("bar", st2.dim_name(1));
  ASSERT_EQ("baz", st2.dim_name(2));

  ASSERT_EQ("", st1.dim_name(0));
  ASSERT_EQ("", st1.dim_name(1));
  ASSERT_EQ("", st1.dim_name(2));

  const int64_t* ptr = reinterpret_cast<const int64_t*>(st1.raw_data());
  for (int i = 0; i < 6; ++i) {
    ASSERT_EQ(i + 1, ptr[i]);
  }
  for (int i = 0; i < 6; ++i) {
    ASSERT_EQ(i + 11, ptr[i + 6]);
  }

  const auto& si = internal::checked_cast<const SparseCOOIndex&>(*st1.sparse_index());
  std::shared_ptr<SparseCOOIndex::CoordsTensor> sidx = si.indices();
  ASSERT_EQ(std::vector<int64_t>({12, 3}), sidx->shape());
  ASSERT_TRUE(sidx->is_column_major());

  // (0, 0, 0) -> 1
  ASSERT_EQ(0, sidx->Value({0, 0}));
  ASSERT_EQ(0, sidx->Value({0, 1}));
  ASSERT_EQ(0, sidx->Value({0, 2}));

  // (0, 0, 2) -> 2
  ASSERT_EQ(0, sidx->Value({1, 0}));
  ASSERT_EQ(0, sidx->Value({1, 1}));
  ASSERT_EQ(2, sidx->Value({1, 2}));

  // (0, 1, 1) -> 3
  ASSERT_EQ(0, sidx->Value({2, 0}));
  ASSERT_EQ(1, sidx->Value({2, 1}));
  ASSERT_EQ(1, sidx->Value({2, 2}));

  // (1, 2, 1) -> 15
  ASSERT_EQ(1, sidx->Value({10, 0}));
  ASSERT_EQ(2, sidx->Value({10, 1}));
  ASSERT_EQ(1, sidx->Value({10, 2}));

  // (1, 2, 3) -> 16
  ASSERT_EQ(1, sidx->Value({11, 0}));
  ASSERT_EQ(2, sidx->Value({11, 1}));
  ASSERT_EQ(3, sidx->Value({11, 2}));
}

TEST(TestSparseCSRMatrix, CreationFromNumericTensor2D) {
  std::vector<int64_t> shape = {6, 4};
  std::vector<int64_t> values = {1, 0,  2, 0,  0,  3, 0,  4, 5, 0,  6, 0,
                                 0, 11, 0, 12, 13, 0, 14, 0, 0, 15, 0, 16};
  std::shared_ptr<Buffer> buffer = Buffer::Wrap(values);
  std::vector<std::string> dim_names = {"foo", "bar", "baz"};
  NumericTensor<Int64Type> tensor1(buffer, shape);
  NumericTensor<Int64Type> tensor2(buffer, shape, {}, dim_names);

  SparseTensorImpl<SparseCSRIndex> st1(tensor1);
  SparseTensorImpl<SparseCSRIndex> st2(tensor2);

  CheckSparseIndexFormatType(SparseTensorFormat::CSR, st1);

  ASSERT_EQ(12, st1.non_zero_length());
  ASSERT_TRUE(st1.is_mutable());

  ASSERT_EQ("foo", st2.dim_name(0));
  ASSERT_EQ("bar", st2.dim_name(1));
  ASSERT_EQ("baz", st2.dim_name(2));

  ASSERT_EQ("", st1.dim_name(0));
  ASSERT_EQ("", st1.dim_name(1));
  ASSERT_EQ("", st1.dim_name(2));

  const int64_t* ptr = reinterpret_cast<const int64_t*>(st1.raw_data());
  for (int i = 0; i < 6; ++i) {
    ASSERT_EQ(i + 1, ptr[i]);
  }
  for (int i = 0; i < 6; ++i) {
    ASSERT_EQ(i + 11, ptr[i + 6]);
  }

  const auto& si = internal::checked_cast<const SparseCSRIndex&>(*st1.sparse_index());

  ASSERT_EQ(std::string("SparseCSRIndex"), si.ToString());
  ASSERT_EQ(1, si.indptr()->ndim());
  ASSERT_EQ(1, si.indices()->ndim());

  const int64_t* indptr_begin = reinterpret_cast<const int64_t*>(si.indptr()->raw_data());
  std::vector<int64_t> indptr_values(indptr_begin,
                                     indptr_begin + si.indptr()->shape()[0]);

  ASSERT_EQ(7, indptr_values.size());
  ASSERT_EQ(std::vector<int64_t>({0, 2, 4, 6, 8, 10, 12}), indptr_values);

  const int64_t* indices_begin =
      reinterpret_cast<const int64_t*>(si.indices()->raw_data());
  std::vector<int64_t> indices_values(indices_begin,
                                      indices_begin + si.indices()->shape()[0]);

  ASSERT_EQ(12, indices_values.size());
  ASSERT_EQ(std::vector<int64_t>({0, 2, 1, 3, 0, 2, 1, 3, 0, 2, 1, 3}), indices_values);
}

}  // namespace arrow
