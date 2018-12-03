/*
 *
 *   mssd detect header
 *
 */


typedef void* tensor_t;
typedef void* graph_t;
// create graph & ready to run graph
void post_process_ssd(const char* image_file,float threshold,float* global_out_data,int numb_of_obj, const char* save_name);
int graph_ready(graph_t *global_graph,tensor_t *global_input_tensor, int* glbal_dim, const char *model_name,const char* model_path,const char* proto_file_path,const char* device_type);
int detect(float* global_input_data,float** global_out_data, graph_t global_graph, tensor_t global_input_tensor, tensor_t *global_out_tensor ,int* num,int img_size);
void graph_finish(float* global_input_data, graph_t global_graph, tensor_t global_input_tensor, const char* model_name);
int delete_out_tensor(tensor_t global_out_tensor);




