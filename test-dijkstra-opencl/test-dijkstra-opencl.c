#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#ifdef __APPLE__
  #include <OpenCL/cl.h>
#else
  #include <CL/cl.h>
#endif

void check_error_file_line(int err_num, int expected, const char* file, const
    int line_number);

#define check_error(a, b) check_error_file_line(a, b, __FILE__, __LINE__)

void usage()
{
  printf("Usage:test-dijkstra edge_matrix_file weight_matrix_file width, height, source_i source_j\n");
}

char *error_desc(int err_num)
{
  switch(err_num) {
    case CL_SUCCESS:
      return "CL_SUCCESS";
      break;

    case CL_DEVICE_NOT_FOUND:
      return "CL_DEVICE_NOT_FOUND";
      break;

    case CL_DEVICE_NOT_AVAILABLE:
      return "CL_DEVICE_NOT_AVAILABLE";
      break;

    case CL_COMPILER_NOT_AVAILABLE:
      return "CL_COMPILER_NOT_AVAILABLE";
      break;

    case CL_MEM_OBJECT_ALLOCATION_FAILURE:
      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
      break;

    case CL_OUT_OF_RESOURCES:
      return "CL_OUT_OF_RESOURCES";
      break;

    case CL_OUT_OF_HOST_MEMORY:
      return "CL_OUT_OF_HOST_MEMORY";
      break;

    case CL_PROFILING_INFO_NOT_AVAILABLE:
      return "CL_PROFILING_INFO_NOT_AVAILABLE";
      break;

    case CL_MEM_COPY_OVERLAP:
      return "CL_MEM_COPY_OVERLAP";
      break;

    case CL_IMAGE_FORMAT_MISMATCH:
      return "CL_IMAGE_FORMAT_MISMATCH";
      break;

    case CL_IMAGE_FORMAT_NOT_SUPPORTED:
      return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
      break;

    case CL_BUILD_PROGRAM_FAILURE:
      return "CL_BUILD_PROGRAM_FAILURE";
      break;

    case CL_MAP_FAILURE:
      return "CL_MAP_FAILURE";
      break;

    case CL_INVALID_VALUE:
      return "CL_INVALID_VALUE";
      break;

    case CL_INVALID_DEVICE_TYPE:
      return "CL_INVALID_DEVICE_TYPE";
      break;

    case CL_INVALID_PLATFORM:
      return "CL_INVALID_PLATFORM";
      break;

    case CL_INVALID_DEVICE:
      return "CL_INVALID_DEVICE";
      break;

    case CL_INVALID_CONTEXT:
      return "CL_INVALID_CONTEXT";
      break;

    case CL_INVALID_QUEUE_PROPERTIES:
      return "CL_INVALID_QUEUE_PROPERTIES";
      break;

    case CL_INVALID_COMMAND_QUEUE:
      return "CL_INVALID_COMMAND_QUEUE";
      break;

    case CL_INVALID_HOST_PTR:
      return "CL_INVALID_HOST_PTR";
      break;

    case CL_INVALID_MEM_OBJECT:
      return "CL_INVALID_MEM_OBJECT";
      break;

    case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:
      return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
      break;

    case CL_INVALID_IMAGE_SIZE:
      return "CL_INVALID_IMAGE_SIZE";
      break;

    case CL_INVALID_SAMPLER:
      return "CL_INVALID_SAMPLER";
      break;

    case CL_INVALID_BINARY:
      return "CL_INVALID_BINARY";
      break;

    case CL_INVALID_BUILD_OPTIONS:
      return "CL_INVALID_BUILD_OPTIONS";
      break;

    case CL_INVALID_PROGRAM:
      return "CL_INVALID_PROGRAM";
      break;

    case CL_INVALID_PROGRAM_EXECUTABLE:
      return "CL_INVALID_PROGRAM_EXECUTABLE";
      break;

    case CL_INVALID_KERNEL_NAME:
      return "CL_INVALID_KERNEL_NAME";
      break;

    case CL_INVALID_KERNEL_DEFINITION:
      return "CL_INVALID_KERNEL_DEFINITION";
      break;

    case CL_INVALID_KERNEL:
      return "CL_INVALID_KERNEL";
      break;

    case CL_INVALID_ARG_INDEX:
      return "CL_INVALID_ARG_INDEX";
      break;

    case CL_INVALID_ARG_VALUE:
      return "CL_INVALID_ARG_VALUE";
      break;

    case CL_INVALID_ARG_SIZE:
      return "CL_INVALID_ARG_SIZE";
      break;

    case CL_INVALID_KERNEL_ARGS:
      return "CL_INVALID_KERNEL_ARGS";
      break;

    case CL_INVALID_WORK_DIMENSION:
      return "CL_INVALID_WORK_DIMENSION";
      break;

    case CL_INVALID_WORK_GROUP_SIZE:
      return "CL_INVALID_WORK_GROUP_SIZE";
      break;

    case CL_INVALID_WORK_ITEM_SIZE:
      return "CL_INVALID_WORK_ITEM_SIZE";
      break;

    case CL_INVALID_GLOBAL_OFFSET:
      return "CL_INVALID_GLOBAL_OFFSET";
      break;

    case CL_INVALID_EVENT_WAIT_LIST:
      return "CL_INVALID_EVENT_WAIT_LIST";
      break;

    case CL_INVALID_EVENT:
      return "CL_INVALID_EVENT";
      break;

    case CL_INVALID_OPERATION:
      return "CL_INVALID_OPERATION";
      break;

    case CL_INVALID_GL_OBJECT:
      return "CL_INVALID_GL_OBJECT";
      break;

    case CL_INVALID_BUFFER_SIZE:
      return "CL_INVALID_BUFFER_SIZE";
      break;

    case CL_INVALID_MIP_LEVEL:
      return "CL_INVALID_MIP_LEVEL";
      break;

    case CL_INVALID_GLOBAL_WORK_SIZE:
      return "CL_INVALID_GLOBAL_WORK_SIZE";
      break;
  }
  return "";
}

cl_program load_and_build_program(cl_context context, cl_device_id device, char *file_name)
{
  cl_program program;
  int program_size;
  cl_int err_num;

  FILE *program_f;

  char *program_buffer;

  program_f = fopen(file_name, "r");

  fseek(program_f, 0, SEEK_END);

  program_size = ftell(program_f);
  rewind(program_f);

  program_buffer = malloc(program_size + 1);
  program_buffer[program_size] = '\0';

  fread(program_buffer, sizeof(char), program_size, program_f);
  fclose(program_f);

  program = clCreateProgramWithSource(context, 1, (const char **)&program_buffer,
      NULL, &err_num);

  check_error(err_num, CL_SUCCESS);

  err_num = clBuildProgram(program, 0, NULL, "-g", NULL, NULL);
  if (err_num != CL_SUCCESS)
   {
     size_t size;

      err_num = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL,
          &size);
      check_error(err_num, CL_SUCCESS);

      char *log = malloc(size+1);

      err_num = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, size, log,
          NULL);

      fprintf(stderr, "%s\n", log);

      check_error(err_num, CL_SUCCESS);
   }

  return program;
}

int round_worksize_up(int group_size, int global_size)
{
  int remainder = global_size % group_size;
  if (!remainder)
    return global_size;
  else
    return global_size + group_size - remainder;
}

void check_error_file_line(int err_num, int expected, const char* file, const
    int line_number)
{
  if (err_num != expected)
    {
      fprintf(stderr, "Line %d in File %s:", line_number, file);
      fprintf(stderr, "%s\n", error_desc(err_num));
      exit(1);
    }
}
int mask_array_empty(int *maskArray, int count)
{
  int i;
  for(i = 0; i < count; i++)
  {
      if (maskArray[i] == 1)
      {
          return 0;
      }
  }

  return 1;
}

void print_matrix(int *matrix, int size)
{
  int i;
  for (i=0; i<size; i++)
    {
      printf("%d ", matrix[i]);
    }
  printf("\n");
}

void dijkstra_reference(int *edges, int *weights, int source_i, int source_j,
    int *costs, int width, int height)
{
  unsigned int i, j, size, *mask_array;

  size = width*height;
  mask_array = malloc (size * sizeof (unsigned int));

  for (i=0; i < width; i++)
  {
    for (j=0; j < height; j++)
      {
        if (i==source_i && j==source_j)
          {
            costs[j*width + i] = 0;
            mask_array[j*width + i] = 1;
          }
        else
          {
            mask_array[j*width + i] = 0;
          }
      }
  }

  while (!mask_array_empty (mask_array, size))
    {
      int tid;
      for (tid = 0; tid < size; tid++)
        {
          if (mask_array[tid] != 0)
            {
              mask_array[tid] = 0;

              int edgeStart = tid * 8;
              int edge;
              for (edge=edgeStart; ((edge - edgeStart) < 8) && (edges[edge] !=
                    -1); edge++)
                {
                  int nid = edges[edge];
                  if (costs[nid] > (costs[tid] + weights[edge]) ||
                      costs[nid] == -1)
                    {
                      costs[nid] = costs[tid] + weights[edge];
                      mask_array[nid] = 1;
                    }
                }
            }
        }
    }

  free(mask_array);
  return;
}


cl_int ocl_set_up_context(cl_device_type device_type, cl_platform_id *platform, cl_context *context,
    cl_device_id *device, cl_command_queue *command_queue)
{
  cl_int err_num;

  cl_context_properties contextProperties[] =
    {
      CL_CONTEXT_PLATFORM,
      (cl_context_properties)*platform,
      0
    };

  *context = clCreateContextFromType(contextProperties, device_type,
      NULL, NULL, &err_num);

  if (err_num != CL_SUCCESS)
    {
      /* FIXME add rest of the devices */
      if (device_type == CL_DEVICE_TYPE_CPU)
        printf("No CPU devices found.\n");
      else if (device_type == CL_DEVICE_TYPE_GPU)
        printf("No GPU devices found.\n");
    }

  err_num = clGetDeviceIDs(*platform, device_type, 1, device, NULL);

  if (err_num != CL_SUCCESS)
    {
      return err_num;
    }

  *command_queue = clCreateCommandQueue(*context, *device, 0, &err_num);

  if (err_num != CL_SUCCESS)
    {
      return err_num;
    }
}

int get_longer_distance(int *edge_matrix, int *distances, int matrix_size)
{
  int i, node, farthest_node;

  farthest_node = -1;
  for (i=0; i<matrix_size; i++)
    {
      node = i;
      if (edge_matrix[node*8] != -1 &&
          distances[farthest_node] < distances[node])
        {
          farthest_node = node;
        }
    }
  return farthest_node;
}

int main(int argc, char **argv)
{
  if (argc < 7)
    {
      usage();
      return;
    }

  FILE *edge_f, *weight_f;
  int source_i, source_j, source_vertex;
  int width, height;
  char *line = malloc(sizeof(char)*10);
  int i;

  cl_int err_num;
  cl_platform_id platform;
  cl_device_id device;
  cl_context context;
  cl_command_queue command_queue;

  cl_program program;

  int num_platforms;

  edge_f = fopen(argv[1], "r");
  weight_f = fopen(argv[2], "r");
  width = atoi(argv[3]);
  height = atoi(argv[4]);

  source_i = atoi(argv[5]);
  source_j = atoi(argv[6]);

  source_vertex = source_j*width + source_i;

  int matrix_size = width*height;

  int *edge_matrix = malloc(matrix_size * 8 * sizeof(int));
  int *weight_matrix = malloc(matrix_size * 8 * sizeof(int));
  int *cost_matrix = malloc(matrix_size * sizeof(int));
  int *updating_cost_matrix = malloc(matrix_size * sizeof(int));
  int *mask_matrix = malloc(matrix_size * sizeof(int));

  int *previous_matrix = malloc(matrix_size * sizeof(int));

  for (i=0; i<matrix_size; i++)
    {
      if (i == source_vertex)
        {
          cost_matrix[i] = 0;
          updating_cost_matrix[i] = 0;
        }
      else
        {
          cost_matrix[i] = INT_MAX;
          updating_cost_matrix[i] = INT_MAX;
        }
    }

  i = 0;

  while (fgets(line, 10, edge_f) != NULL) {
    edge_matrix[i] = atoi(line);
    i++;
  }

  i = 0;

  while (fgets(line, 10, weight_f) != NULL) {
    weight_matrix[i] = atoi(line);
    i++;
  }
  err_num = clGetPlatformIDs(1, &platform, &num_platforms);

  if (err_num != CL_SUCCESS || num_platforms <= 0)
    {
      printf("Failed to find any OpenCL platforms.\n");
      return 1;
    }

  err_num = ocl_set_up_context(CL_DEVICE_TYPE_CPU, &platform, &context, &device, &command_queue);
  check_error(err_num, CL_SUCCESS);

  program = load_and_build_program(context, device, "dijkstra.cl");


/*
  size_t max_workgroup_size;

  err_num = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t),
      &max_workgroup_size, NULL);

  check_error(err_num, CL_SUCCESS);

  size_t local_worksize = max_workgroup_size;
  size_t global_worksize = round_worksize_up(local_worksize, matrix_size);
*/
  cl_mem edge_matrix_device;
  cl_mem weight_matrix_device;
  cl_mem mask_matrix_device;
  cl_mem cost_matrix_device;
  cl_mem updating_cost_matrix_device;
  cl_mem previous_matrix_device;

  edge_matrix_device = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int)
      * matrix_size * 8, NULL, &err_num);
  check_error(err_num, CL_SUCCESS);

  weight_matrix_device = clCreateBuffer(context, CL_MEM_READ_ONLY,
      sizeof(int) * matrix_size * 8, NULL, &err_num);
  check_error(err_num, CL_SUCCESS);

  mask_matrix_device = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)
      * matrix_size, NULL, &err_num);
  check_error(err_num, CL_SUCCESS);

  cost_matrix_device = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)
      * matrix_size, NULL, &err_num);
  check_error(err_num, CL_SUCCESS);

  updating_cost_matrix_device = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(int)
      * matrix_size, NULL, &err_num);
  check_error(err_num, CL_SUCCESS);

  previous_matrix_device = clCreateBuffer(context, CL_MEM_READ_WRITE,
      sizeof(int) * matrix_size, NULL, &err_num);
  check_error(err_num, CL_SUCCESS);


  err_num = clEnqueueWriteBuffer(command_queue, edge_matrix_device, CL_TRUE, 0,
      sizeof(int) * matrix_size*8, edge_matrix, 0, NULL, NULL);
  check_error(err_num, CL_SUCCESS);
  err_num = clEnqueueWriteBuffer(command_queue, weight_matrix_device, CL_TRUE, 0,
      sizeof(int) * matrix_size*8, weight_matrix, 0, NULL, NULL);
  check_error(err_num, CL_SUCCESS);
  err_num = clEnqueueWriteBuffer(command_queue, cost_matrix_device, CL_TRUE, 0,
      sizeof(int) * matrix_size, cost_matrix, 0, NULL, NULL);
  check_error(err_num, CL_SUCCESS);
  err_num = clEnqueueWriteBuffer(command_queue, updating_cost_matrix_device, CL_TRUE, 0,
      sizeof(int) * matrix_size, updating_cost_matrix, 0, NULL, NULL);
  check_error(err_num, CL_SUCCESS);

  size_t local_worksize = 1;
  size_t global_worksize = matrix_size;
  cl_event read_done;

  cl_kernel initialize_buffers_kernel;
  initialize_buffers_kernel = clCreateKernel(program, "initialize_buffers",
      &err_num);
  check_error(err_num, CL_SUCCESS);

  err_num |= clSetKernelArg(initialize_buffers_kernel, 0, sizeof(cl_mem),
      &mask_matrix_device);
  err_num |= clSetKernelArg(initialize_buffers_kernel, 1, sizeof(cl_mem),
      &cost_matrix_device);
  err_num |= clSetKernelArg(initialize_buffers_kernel, 2, sizeof(cl_mem),
      &updating_cost_matrix_device);
  err_num |= clSetKernelArg(initialize_buffers_kernel, 3, sizeof(cl_mem),
      &previous_matrix_device);
  err_num |= clSetKernelArg(initialize_buffers_kernel, 4, sizeof(int), &source_vertex);
  check_error(err_num, CL_SUCCESS);

  cl_kernel set_longer_node_kernel;
  set_longer_node_kernel = clCreateKernel(program, "set_longer_node",
      &err_num);
  check_error(err_num, CL_SUCCESS);

  err_num |= clSetKernelArg(set_longer_node_kernel, 0, sizeof(cl_mem),
      &cost_matrix_device);
  check_error(err_num, CL_SUCCESS);

  cl_kernel dijkstra_kernel1;
  dijkstra_kernel1 = clCreateKernel(program, "dijkstra1", &err_num);

  check_error(err_num, CL_SUCCESS);
  cl_kernel dijkstra_kernel2;

  dijkstra_kernel2 = clCreateKernel(program, "dijkstra2", &err_num);

  check_error(err_num, CL_SUCCESS);

  err_num |= clSetKernelArg(dijkstra_kernel1, 0, sizeof(cl_mem), &edge_matrix_device);
  err_num |= clSetKernelArg(dijkstra_kernel1, 1, sizeof(cl_mem), &weight_matrix_device);
  err_num |= clSetKernelArg(dijkstra_kernel1, 2, sizeof(cl_mem), &mask_matrix_device);
  err_num |= clSetKernelArg(dijkstra_kernel1, 3, sizeof(cl_mem), &cost_matrix_device);
  err_num |= clSetKernelArg(dijkstra_kernel1, 4, sizeof(cl_mem), &updating_cost_matrix_device);
  err_num |= clSetKernelArg(dijkstra_kernel1, 5, sizeof(cl_mem),
      &previous_matrix_device);
  err_num |= clSetKernelArg(dijkstra_kernel1, 6, sizeof(int), &matrix_size);
  check_error(err_num, CL_SUCCESS);

  err_num |= clSetKernelArg(dijkstra_kernel2, 0, sizeof(cl_mem),
      &cost_matrix_device);
  err_num |= clSetKernelArg(dijkstra_kernel2, 1, sizeof(cl_mem),
      &updating_cost_matrix_device);
  err_num |= clSetKernelArg(dijkstra_kernel2, 2, sizeof(cl_mem),
      &mask_matrix_device);
  check_error(err_num, CL_SUCCESS);

  int nr_nodes, node;
  for (nr_nodes = 3; nr_nodes>0; nr_nodes--)
    {
      err_num |= clSetKernelArg(initialize_buffers_kernel, 4, sizeof(int),
          &source_vertex);
      check_error(err_num, CL_SUCCESS);

      err_num = clEnqueueNDRangeKernel(command_queue, initialize_buffers_kernel, 1, NULL,
      &global_worksize, NULL, 0, NULL, NULL);
      check_error(err_num, CL_SUCCESS);

      err_num = clEnqueueReadBuffer(command_queue, mask_matrix_device, CL_FALSE, 0,
          sizeof(int) * matrix_size, mask_matrix, 0, NULL, &read_done);
      check_error(err_num, CL_SUCCESS);

      clWaitForEvents(1, &read_done);

      while (!mask_array_empty(mask_matrix, matrix_size))
        {
          err_num = clEnqueueNDRangeKernel(command_queue, dijkstra_kernel1, 1, NULL,
              &global_worksize, NULL, 0, NULL, NULL);
          check_error(err_num, CL_SUCCESS);

          err_num = clEnqueueNDRangeKernel(command_queue, dijkstra_kernel2, 1, NULL,
              &global_worksize, NULL, 0, NULL, NULL);
          check_error(err_num, CL_SUCCESS);

          err_num = clEnqueueReadBuffer(command_queue, mask_matrix_device, CL_FALSE, 0,
          sizeof(int) * matrix_size, mask_matrix, 0, NULL, &read_done);
          check_error(err_num, CL_SUCCESS);

          clWaitForEvents(1, &read_done);
        }
      err_num = clEnqueueReadBuffer(command_queue, cost_matrix_device, CL_FALSE, 0,
                sizeof(int) * matrix_size, cost_matrix, 0, NULL, &read_done);

      check_error(err_num, CL_SUCCESS);

      clWaitForEvents(1, &read_done);

      err_num |= clSetKernelArg(set_longer_node_kernel, 1, sizeof(int), &node);

      check_error(err_num, CL_SUCCESS);
      node = get_longer_distance(edge_matrix, cost_matrix, matrix_size);

      err_num |= clSetKernelArg(set_longer_node_kernel, 1, sizeof(int), &node);
      check_error(err_num, CL_SUCCESS);

      err_num = clEnqueueNDRangeKernel(command_queue, set_longer_node_kernel,
          1, NULL, &global_worksize, NULL, 0, NULL, NULL);

      source_vertex = node;
    }
  int j, cost;

  /*err_num = clEnqueueReadBuffer(command_queue, previous_matrix_device,
      CL_FALSE, 0, sizeof(int) * matrix_size, previous_matrix, 0, NULL,
      &read_done);
  check_error(err_num, CL_SUCCESS);

  clWaitForEvents(err_num, CL_SUCCESS);*/
/*
  printf("\n\n\n");

  int j, cost;

  for (i=0; i < width; i++)
    {
      for (j=0; j < height; j++)
        {
          printf("%d ", previous_matrix[j*width+i]);
        }
      printf("\n");
    }
*/
/*  dijkstra_reference(edge_matrix, weight_matrix, source_i, source_j,
      cost_matrix, width, height);*/

  err_num = clEnqueueReadBuffer(command_queue, cost_matrix_device, CL_FALSE, 0,
          sizeof(int) * matrix_size, cost_matrix, 0, NULL, &read_done);

  check_error(err_num, CL_SUCCESS);

  clWaitForEvents(1, &read_done);

  for (i=0; i < width; i++)
    {
      for (j=0; j < height; j++)
        {
          if ((cost = cost_matrix[j*width+i]) == INT_MAX)
            {
              printf("-1 ");
            }
          else
            {
              printf("%d ", cost_matrix[j*width + i]);
            }
        }
      printf("\n");
    }
  printf("\n\n\n");

  free(edge_matrix);
  free(weight_matrix);
  free(cost_matrix);
  free(mask_matrix);

  clReleaseMemObject(edge_matrix_device);

  return 0;
}
