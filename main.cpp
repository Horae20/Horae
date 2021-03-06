//测试边查、点查等结果
#include "QueryFunction.h"
#include <iomanip>

time_type getDatasetStartTime(string datasetPath);

int main(int argc, char* argv[]) {
	//强制使用小数,防止使用科学计数法
    cout << fixed;
    //控制显示的精度，控制小数点后面的位数
#if defined(DEBUG) || defined(HINT)  
	cout << setprecision(7);
	timeval main_start, main_end;
	gettimeofday( &main_start, NULL);

	for (int i = 0; i < argc; i++) {
		cout << argv[i] << " ";
	}
	cout << endl << endl;
#endif
	//HORAE_VAR varibles  //int granularityLength = 43200;  //int granularityLength = 604800;// 2592000 one month
	time_type startTime;
	uint32_t width, depth;
	uint32_t granularityLength = 61200, gl = 1, slot = 2, fingerprintLen = 7;
	// string back_addr = "_Horae-1-addr_res_FINAL";
#ifdef KICK_2x8
	string back_addr = "_Horae-2x8-kick-res";
#endif
#ifdef NO_KICK_2x8
	string back_addr = "_Horae-2x8-no-kick-res";
#endif
#ifdef KICK_4x4
	string back_addr = "_Horae-4x4-kick-res";
#endif
#ifdef NO_KICK_4x4
	string back_addr = "_Horae-4x4-no-kick-res";
#endif
#ifdef ADDR_1
	string back_addr = "_Horae-1-addr-res";
#endif
	int dataset = 3;
	int test_situation = 1;						//0-baseline，1-pgss
	int query_times = 1;						//查询的次数
	string filename, input_dir, output_dir;		//测试数据集文件路径  测试数据输入路径 测试结果输出路径
	string dataset_name, txt_name = "";
	vector<int> num;
	int efflag = 0, eeflag = 0, nfflag = 0; 	//  edge frequence query,  edge existence query,  node frequence query
	bool parallel_insert = false;				//是否并行插入
	bool writeflag = false;						//是否将测试结果写入到文件
	int node_query_flag = 0;					//1-node_in_query, 2-node_out_query
	int edge_existence_flag = 1;				//1-edge_existence_query, 2-bool_query
	int line = 0;
	bool para_query = true;  						//0-串行结果查询，1-并行结果查询
	//命令行参数
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-line") == 0) {
			line = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-dataset") == 0) {
			dataset = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-gl") == 0) {
			granularityLength = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-slot") == 0) {
			slot = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-fplength") == 0) {
			fingerprintLen = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-edgeweight") == 0) {
			efflag = 1;
		}
		if (strcmp(argv[i], "-edgeexistence") == 0) {
			eeflag = 1;
		}
		if (strcmp(argv[i], "-nodeinweight") == 0) {
			nfflag = 1;
			node_query_flag = 1;
		}
		if (strcmp(argv[i], "-nodeoutweight") == 0) {
			node_query_flag = 1;
			node_query_flag = 2;
		}
		if (strcmp(argv[i], "-write") == 0) {
			writeflag = true;
		}
		if (strcmp(argv[i], "-qtimes") == 0) {
			query_times = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-baseline") == 0) {
			test_situation = 0;
			txt_name = "baseline";
		}
		if (strcmp(argv[i], "-pgss") == 0) {
			test_situation = 1;
			txt_name = "pgss";
		}
		if (strcmp(argv[i], "-para") == 0) {
			parallel_insert = true;
		}
		if (strcmp(argv[i], "-seq") == 0) {
			parallel_insert = false;
		}
		if (strcmp(argv[i], "-bool") == 0) {
			edge_existence_flag = 2;
		}
		if (strcmp(argv[i], "-para_query") == 0) {
			para_query = true;
		}
		if (strcmp(argv[i], "-seq_query") == 0) {
			para_query = false;
		}
	}

	switch (dataset) {//数据集_查询类型_baseline/pgss_长度_res.txt
		case 1:
			filename = "..//..//Dataset//out";
			input_dir = "..//..//TestFiles//out//input//";
			output_dir = "..//..//TestFiles//out//output//";
			dataset_name = "out";
			num = { 8, 16, 32, 64, 128, 256, 384 };
			if (test_situation == 0 || test_situation == 2) { //baseline or single dynamic pgss
				width = 2850;
				depth = 3158;
			}
			else if (test_situation == 1) {  //pgss
				width = 900;
				depth = 1000;
			}
			break;
		case 2:
			filename = "..//..//Dataset//lastfm_song";
			input_dir = "..//..//TestFiles//lastfm_song//input//";
			output_dir = "..//..//TestFiles//lastfm_song//output//";
			dataset_name = "lastfm_song";
			num = { 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560, 3072};
			if (test_situation == 0 || test_situation == 2) { //baseline or single dynamic pgss
				width = 45000;
				depth = 1800;
			}
			else if (test_situation == 1) {  //pgss
				width = 15000;
				depth = 600;
			}
			break;
		case 3:
			filename = "..//..//Dataset//stackoverflow";
			input_dir = "..//..//TestFiles//stackoverflow//input//";
			output_dir = "..//..//TestFiles//stackoverflow//output//";
			dataset_name = "stackoverflow";
			//num = { 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560, 3072, 3584 };
			num = { 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560 };
			if (test_situation == 0 || test_situation == 2) { //baseline or single dynamic pgss
				width = 20396;/////////
				depth = 20397;/////////
			}
			else if (test_situation == 1) {  //pgss
				width = 5655;///////////
				depth = 5659;///////////
			}
			break;
		case 4:
			filename = "..//..//Dataset//caida";
			input_dir = "..//..//TestFiles//caida//input//";
			output_dir = "..//..//TestFiles//caida//output//";
			dataset_name = "caida";
			num = { 1024, 2048, 3072, 4096, 5120, 6144, 7168, 8192, 9216, 10240, 11264, 12288 };
			if (test_situation == 0 || test_situation == 2) { //baseline or single dynamic pgss
				width = 54000;
				depth = 54000;
			}
			else if (test_situation == 1) {  //pgss
				width = 15000;
				depth = 15000;
			}
			break;
		case 5:
			filename = "..//..//Dataset//wiki";
			input_dir = "..//..//TestFiles//wiki//input//";
			output_dir = "..//..//TestFiles//wiki//output//";
			dataset_name = "wiki";
			num = { 64, 128, 256, 512, 1024, 1536, 2048, 2560, 3072, 3584, 4096, 4608 };
			if (test_situation == 0 || test_situation == 2) { //baseline or single dynamic pgss
				width = 6500;
				depth = 6550;
			}
			else if (test_situation == 1) {  //pgss
				width = 1750;
				depth = 1800;
			}
			break;
		case 6:
			filename = "..//..//Dataset//lkml";
			input_dir = "..//..//TestFiles//lkml-10w-86400//input//";
			output_dir = "..//..//TestFiles//lkml-10w-86400//output//";
			dataset_name = "lkml";
			num = { 8, 16, 32, 64, 128, 256, 512, 1024, 1536, 2048, 2560 };
			if (test_situation == 0 || test_situation == 2) { //baseline or single dynamic pgss
					width = 6500;
					depth = 6550;
			}
			else if (test_situation == 1) {  //pgss
					width = 1750;
					depth = 1800;
			}
			break;
		default:
			break;
	}

	//命令行参数
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-vector") == 0) {
			num.clear();
			while(true) {
				if((i + 1) >= argc) {
					break;
				}
				if(argv[i + 1][0] < '0' || argv[i + 1][0] > '9') {
					break;
				}
				else {
					num.push_back(atoi(argv[++i]));
				}
			}
		}
		if (strcmp(argv[i], "-input_dir") == 0) {
			input_dir = argv[++i];
			input_dir += "//";
		}
		if (strcmp(argv[i], "-output_dir") == 0) {
			output_dir = argv[++i];
			output_dir += "//";
		}
		if (strcmp(argv[i], "-filename") == 0) {
			filename = argv[++i];
		}
		if (strcmp(argv[i], "-w") == 0) {
			width = atoi(argv[++i]);
		}
		if (strcmp(argv[i], "-d") == 0) {
			depth = atoi(argv[++i]);
		}
	}
	startTime = getDatasetStartTime(filename);
	HORAE_VAR horae_var = { startTime, granularityLength, gl, width, depth, fingerprintLen };
#if defined(DEBUG) || defined(HINT)
	cout << "*******************************************************" << endl;
	cout << "Print Infomation" << endl;
	cout << "HORAE_VAR: startTime = " << horae_var.startTime 
		 << ", granularityLength = " << horae_var.granularityLength 
		 << ", gl = " << horae_var.gl 
		 << ", width = " << horae_var.width 
		 << ", depth = " << horae_var.depth 
		 << ", fingerprintLen = " << horae_var.fingerprintLen << endl;
#endif
	string test_situation_dir = dataset_name + "_gl_" + to_string(horae_var.granularityLength) + "_" + to_string(horae_var.width) + "_" + 
		to_string(horae_var.depth) + "_" + to_string(SLOTNUM) + "_" + to_string(horae_var.fingerprintLen) + back_addr +"//";
	output_dir += test_situation_dir;

	char dir_path[FILENAME_MAX];
	strcpy(dir_path, output_dir.c_str());
	if (createDirectory(dir_path) != 0) {
		cout << "Create Directory error" << endl;
		return -1;
	}
#if defined(DEBUG) || defined(HINT)
	cout << "dataset: " << filename << endl;
	cout << "input_dir: " << input_dir << endl;
	cout << "output_dir: " << output_dir << endl;
	cout << "write flag = " << writeflag << endl;
	cout << "vector num: ";
	for (int i = 0; i < num.size(); i++) {
		cout << num[i] << " ";
	}
	cout << endl;
	cout << "*******************************************************" << endl << endl;
#endif
	switch (test_situation) {
		case 0:
#if defined(DEBUG) || defined(HINT)
			cout << "****************** baseline insert start *****************" << endl;
#endif
			baselineInsert(horae_var, filename);
#if defined(DEBUG) || defined(HINT)
			cout << "****************** baseline insert end *******************" << endl << endl;
#endif
			if (efflag == 1) {
#if defined(DEBUG) || defined(HINT)
				cout << "**************** baseline frequence start ****************" << endl;
#endif
				edgeFrequenceBaselineTest(para_query, input_dir, output_dir, dataset_name, num, query_times, writeflag);
#if defined(DEBUG) || defined(HINT)
				cout << "***************** baseline frequence end *****************" << endl << endl;
#endif
			}
			if (eeflag == 1) {
#if defined(DEBUG) || defined(HINT)
				cout << "**************** baseline existence start ****************" << endl;
#endif
				edgeExistenceBaselineTest(para_query, input_dir, output_dir, dataset_name, num, query_times, writeflag, edge_existence_flag);
#if defined(DEBUG) || defined(HINT)
				cout << "***************** baseline existence end *****************" << endl << endl;
#endif
			}
			if (nfflag == 1) {
#if defined(DEBUG) || defined(HINT)
				cout << "************* baseline node frequence start **************" << endl;
#endif
				nodeFrequenceBaselineTest(para_query, input_dir, output_dir, dataset_name, num, query_times, writeflag, node_query_flag, line);
#if defined(DEBUG) || defined(HINT)
				cout << "************** baseline node frequence end ***************" << endl << endl;
#endif
			}
			break;
		case 1:
			if (parallel_insert) {
#if defined(DEBUG) || defined(HINT)
				cout << "****************** pgss parallel insert start *****************" << endl;
				timeval t_start, t_end;
				gettimeofday( &t_start, NULL);
#endif
				pgssParallelInsert(horae_var, filename);
#if defined(DEBUG) || defined(HINT)
				gettimeofday( &t_end, NULL);
				double delta_t = (t_end.tv_sec-t_start.tv_sec) + 
						(t_end.tv_usec-t_start.tv_usec)/1000000.0;
				cout << "all time : " << delta_t  << "s" << endl;
				cout << "****************** pgss parallel insert end *******************" << endl << endl;
#endif
			}
			else {
#if defined(DEBUG) || defined(HINT)
				cout << "****************** pgss sequential insert start *****************" << endl;
#endif
				pgssSequentialInsert(horae_var, filename);
#if defined(DEBUG) || defined(HINT)
				cout << "****************** pgss sequential insert end *******************" << endl << endl;
#endif
			}
			if (efflag == 1) {
#if defined(DEBUG) || defined(HINT)
				cout << "**************** pgss frequence start ****************" << endl;
#endif
				if (parallel_insert) {
					edgeFrequencePgssTest(para_query, pgss_parallel, input_dir, output_dir, dataset_name, num, query_times, writeflag);
				}
				else {
					edgeFrequencePgssTest(para_query, pgss_sequential, input_dir, output_dir, dataset_name, num, query_times, writeflag);
				}
#if defined(DEBUG) || defined(HINT)
				cout << "***************** pgss frequence end *****************" << endl << endl;
#endif
			}
			if (eeflag == 1) {
#if defined(DEBUG) || defined(HINT)
				cout << "**************** pgss existence start ****************" << endl;
#endif
				if (parallel_insert) {
					edgeExistencePgssTest(para_query,pgss_parallel, input_dir, output_dir, dataset_name, num, query_times, writeflag, edge_existence_flag);
				}
				else {
					edgeExistencePgssTest(para_query, pgss_sequential, input_dir, output_dir, dataset_name, num, query_times, writeflag, edge_existence_flag);
				}
#if defined(DEBUG) || defined(HINT)
				cout << "***************** pgss existence end *****************" << endl << endl;
#endif
			}
			if (nfflag == 1) {
#if defined(DEBUG) || defined(HINT)
				cout << "************* pgss node frequence start **************" << endl;
#endif
				if (parallel_insert) {
					nodeFrequencePgssTest(para_query, pgss_parallel, input_dir, output_dir, dataset_name, num, query_times, writeflag, node_query_flag, line);
				}
				else {
					nodeFrequencePgssTest(para_query, pgss_sequential, input_dir, output_dir, dataset_name, num, query_times, writeflag, node_query_flag, line);
				}
#if defined(DEBUG) || defined(HINT)
				cout << "************** pgss node frequence end ***************" << endl << endl;
#endif
			}
			break;
		default:
			break;
	}
#if defined(DEBUG) || defined(HINT)
	gettimeofday( &main_end, NULL);
	double main_time = (main_end.tv_sec - main_start.tv_sec) + (main_end.tv_usec - main_start.tv_usec) / 1000000.0;
	cout << endl << "This program lasts for " << main_time / 60.0 << " min" << endl;
#endif
	return 0;
}

time_type getDatasetStartTime(string datasetPath) {
	ifstream ifs;
	ifs.open(datasetPath);
	if(!ifs.is_open()) {
		cout << "Open dataset error! Path = " << datasetPath << endl;
		return -1;
	}
	int64_t s, d;
	weight_type w;
	time_type startTime;
	ifs >> s >> d >> w >> startTime;
	ifs.close();
	if(startTime > 1) 
		return startTime - 1;
	else
		return -1;
}
