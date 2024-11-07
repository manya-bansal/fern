void my_fused_impl(const Array<float> x, Array<float> getitem, int64_t len1){
 for(int64_t x0 = getitem.idx;x0 < getitem.idx + getitem.size; x0+=len1){
    Array<float> getitem_q = getitem.array_query_no_materialize(x0, len1 + 1 / 2);
  Array<float> x_q1 = x.array_query_no_materialize(x0, len1);
    mock::every_other(x_q1.data, getitem_q.data);
}
}

