#ifndef _SVM_PREDICT_ONLINE
#define _SVM_PREDICT_ONLINE
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include "svm.h"

void predict_online()
{
	double predict_label;
	struct svm_model* model_predict;
	//model_predict初始化
	char *model_file_name = "emg-character-train-test.model";
	if ((model_predict = svm_load_model(model_file_name)) == 0)
	{
		fprintf(stderr, "can't open model file %s\n", model_file_name);
		exit(1);
	}
	struct svm_node *x;
	x = (struct svm_node *) malloc(24 * sizeof(struct svm_node));
	/*for (int i = 0; i < 8; i++)
	{
	x[i].index = i + 1;
	x[i].value = 1;
	}*/
	int i = 0;
	x[i].index = i + 1; x[i++].value = 0.0185622;
	x[i].index = i + 1; x[i++].value = 0.0165746;
	x[i].index = i + 1; x[i++].value = -0.00152989;
	x[i].index = i + 1; x[i++].value = 0.00631858;
	x[i].index = i + 1; x[i++].value = 0.0293584;
	x[i].index = i + 1; x[i++].value = 0.00502934;
	x[i].index = i + 1; x[i++].value = 0.00608365;
	x[i].index = i + 1; x[i++].value = 0.017077;
	x[i].index = i + 1; x[i++].value = 0.0371198;
	x[i].index = i + 1; x[i++].value = 0.0110064;
	x[i].index = i + 1; x[i++].value = 0.000455674;
	x[i].index = i + 1; x[i++].value = 0.00279263;
	x[i].index = i + 1; x[i++].value = 0.0304008;
	x[i].index = i + 1; x[i++].value = 0.0048715;
	x[i].index = i + 1; x[i++].value = 0.0093424;
	x[i].index = i + 1; x[i++].value = 0.0168805;
	x[i].index = i + 1; x[i++].value = 0.048;
	x[i].index = i + 1; x[i++].value = 0.0126582;
	x[i].index = i + 1; x[i++].value = 0.00806452;
	x[i].index = i + 1; x[i++].value = 0.0454545;
	x[i].index = i + 1; x[i++].value = 0.00793651;
	x[i].index = i + 1; x[i++].value = 0.015873;
	x[i].index = i + 1; x[i++].value = 0.025641;
	x[i].index = i + 1; x[i++].value = 0.025641;
	//x初始化
	predict_label = svm_predict(model_predict, x);
	std::cout << predict_label << std::endl;
}
#endif