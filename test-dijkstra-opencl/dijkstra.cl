__kernel void dijkstra1(__global int *edge_array, __global int *weight_array,
    __global int *mask_array, __global int *cost_array, __global int
    *updating_cost_array, int vertexCount)
{
  int tid = get_global_id (0);

  if (mask_array[tid] != 0)
    {
      mask_array[tid] = 0;

      int edgeStart = tid * 8;
      int edge;
      int nid;

      for (edge = edgeStart; ((edge - edgeStart) < 8) && ((nid = edge_array[edge]) !=
            -1); edge++)
        {
          if (updating_cost_array[nid] > (cost_array[tid] + weight_array[edge]) ||
              updating_cost_array[nid] == -1)
            {
              updating_cost_array[nid] = (cost_array[tid] +
                  weight_array[edge]);
            }
        }
    }
}

__kernel void dijkstra2(__global int *cost_array, __global int
    *updating_cost_array, __global int *mask_array)
{
  int tid = get_global_id(0);

  if ((cost_array[tid] > updating_cost_array[tid]) || (cost_array[tid] == -1))
    {
      cost_array[tid] = updating_cost_array[tid];
      mask_array[tid] = 1;
    }

  updating_cost_array[tid] = cost_array[tid];
}

__kernel void initialize_buffers(__global int *mask_array, int source_vertex)
{
  int tid = get_global_id(0);

  if (source_vertex == tid)
    {
      mask_array[tid] = 1;
    }
  else
    {
      mask_array[tid] = 0;
    }
}
