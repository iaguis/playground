__kernel void dijkstra1(__global int *edge_matrix, __global int *weight_matrix,
    __global int *mask_matrix, __global int *cost_matrix, __global int
    *updating_cost_matrix, __global int *previous, int vertexCount)
{
  int tid = get_global_id (0);

  if (mask_matrix[tid] != 0)
    {
      mask_matrix[tid] = 0;

      int edgeStart = tid * 8;
      int edge;
      int nid;

      for (edge = edgeStart; ((edge - edgeStart) < 8) && ((nid = edge_matrix[edge]) !=
            -1); edge++)
        {
          if (updating_cost_matrix[nid] > (cost_matrix[tid] + weight_matrix[edge]))
            {
              updating_cost_matrix[nid] = (cost_matrix[tid] + weight_matrix[edge]);
              if (previous)
                {
                  previous[nid] = tid;
                }
            }
        }
    }
}

__kernel void dijkstra2(__global int *cost_matrix, __global int
    *updating_cost_matrix, __global int *mask_matrix)
{
  int tid = get_global_id(0);

  if (cost_matrix[tid] > updating_cost_matrix[tid])
    {
      cost_matrix[tid] = updating_cost_matrix[tid];
      mask_matrix[tid] = 1;
    }

  updating_cost_matrix[tid] = cost_matrix[tid];
}

__kernel void initialize_buffers(__global int *mask_matrix, __global int *cost_matrix, __global int *updating_cost_matrix, __global int *previous, int source_vertex)
{
  int tid = get_global_id(0);

  if (previous != 0)
    previous[tid] = -1;
  if (tid == source_vertex)
    {
      mask_matrix[tid] = 1;
      cost_matrix[tid] = 0;
      updating_cost_matrix[tid] = 0;
    }
  else
    {
      mask_matrix[tid] = 0;
    }
}

__kernel void set_longer_node(__global int *cost_matrix, int node)
{
  int tid = get_global_id(0);

  if (tid == node)
    {
      cost_matrix[tid] = 0;
    }
}
