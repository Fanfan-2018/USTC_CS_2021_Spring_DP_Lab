#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#define K 2000
int MaxRow;//最大行数
char b[10];//暂存数组
int tmp;//输入下标
double LM;//LM
int writeindex;//输出下标
int Age[33000];//第一列
char CAge[33000][10];//泛化结果
int Gender[33000];//第十列
char CGender[33000][10];//泛化结果
int EducationNum[33000];//第五列
char CEducationNum[33000][10];
char occupation[33000][30];
void Scan(){//同第一个算法的Scan部分
    int i,len;
    int scanflag = 1;
    int row = 0;
    tmp = 0;
    FILE *fpRead=fopen("G:\\360MoveData\\Users\\Lenovo\\Desktop\\adult.data","r");
    if(fpRead==NULL){
		return;
	}
    for(i = 0;i < 490000;i ++){
        //C语言文本文件输入，遇见空格或者换行即停止
        //一行十五列数据，我们需要的只有第一列的Age，第六列的Marital，第七列的Occupation，第九列的Race，第十列的Gender
        //各列均由一个数组存储
        if(fscanf(fpRead,"%s",b) == EOF){
            break;
        }
        row ++;
        //当扫描某一行时，遇到"?"，会允许后续该行属性继续输入，但很快就会被下一行合法数据给覆盖
        //即遇到"?"，不改变输入到数组对应的下标
        if(b[0] == '?'){
            scanflag = 0;
        }
        if(row == 1){
            //第一列Age
            Age[tmp] = (b[0] - '0') * 10 + (b[1] - '0');
        }else if(row == 5){
            //第五列EducationNum
            if(strlen(b) == 3){//为3是因为，我们输入的时候会把逗号也输入b
                //有两位数的情况
                EducationNum[tmp] = (b[0] - '0') * 10 + (b[1] - '0');
            }else{
                //只有一位数的情况
                EducationNum[tmp] = b[0] - '0';
            }
        }else if(row == 7){
            //第七列occupation
            strcpy(occupation[tmp],b);
        }else if(row == 10){
            //第十列Gender 
            if(b[0] == 'M'){
                Gender[tmp] = 0;
            }else{
                Gender[tmp] = 1;
            }
        //判断是否要输入数组下标是否可以加1
        }else if(row == 15 && scanflag == 0){
            //txt中该行存在"?"，tmp不变，下次合法数据直接覆盖
            scanflag = 1;
            row = 0;
        }else if(row == 15 && scanflag == 1){
            //txt中该行为合法数据，tmp++
            tmp ++;
            row = 0;
        }
    }
    MaxRow = tmp;
    return;
}
//快速排序
void QuickSort(int arra[33000], int arrb[33000], char arrc[33000][30] ,int low, int high){
    //事实上只对arra排序，arrb、arrc对应下标进行相同调换
    if (low < high){
        int i = low;
        int j = high;
        int k = arra[low];
        int t = arrb[low];
        char * h;
        h = (char*)malloc(20);
        strcpy(h, arrc[low]);
        while (i < j){
            while(i < j && arra[j] >= k){     // 从右向左找第一个小于k的数
                j--;
            }
            if(i < j){
                arra[i] = arra[j];
                arrb[i] = arrb[j];
                strcpy(arrc[i], arrc[j]);
                i++;
            }
            while(i < j && arra[i] < k){      // 从左向右找第一个大于等于k的数
                i++;
            }
            if(i < j){
                arra[j] = arra[i];
                arrb[j] = arrb[i];
                strcpy(arrc[j],arrc[i]);
                j--;
            }
        }
 
        arra[i] = k;
        arrb[i] = t;
        strcpy(arrc[i], h);
        free(h);
        // 递归调用
        QuickSort(arra, arrb, arrc, low, i - 1);     // 排序k左边
        QuickSort(arra, arrb, arrc, i + 1, high);    // 排序k右边
        return;
    }
}
void Anonymize(int arra[33000], int arrb[33000], char arrc[33000][30], int start, int end, int flag){
    //arra对应Age，arrb对应EducationNum，arrc对应occupation
    //选做时，arra对应Age，arrb对应Gender，arrc对应occupation
    int median,strict,k;//median是绝对中位数 strict是严格中位数
    double tLM = 0.0;
    double sub = 0;
    median = (start + end) / 2;
    if(flag == 0){//arra已排序
        //那么先找arra对应的严格中位数
        for(k = start;k <= end;k ++){
            if(arra[k] > arra[median]){
                break;
            }
        }
        strict = k;
    }else if(flag == 1){//arrb已排序
        //那么先找arrb对应的严格中位数
        for(k = start;k <= end;k ++){
            if(arrb[k] > arrb[median]){
                break;
            }
        }
        strict = k;
    }
    //先判断该维度是否可以继续分割，如果不满足继续分割，那么我们要判读另一个维度
    if(end - strict + 1< K || strict - start < K){
        //转换到另一个维度
        flag ++;
        flag = flag % 2;
        if(flag == 0){ 
            //之前flag是1，现在转换到0，对arra排序
            QuickSort(arra, arrb, arrc, start, end);
            //计算暂时LM，并不一定加入，如果另一个维度也不可分就加入LM
            sub = (arra[end] - arra[start]);
            tLM += (sub / 73.0) * (end - start + 1);
            //现在找arra的严格中位数
            for(k = start;k <= end;k ++){
                if(arra[k] > arra[median]){
                    break;
                }
            }
            strict = k;
        }else{
            //之前flag是0，现在转换到1，对arrb排序
            QuickSort(arrb, arra, arrc, start, end);
            //计算暂时LM，同样并不一定加入LM
            sub = (arrb[end] - arrb[start]);
            tLM += (sub / 15.0) * (end - start + 1);
            //现在找arrb的严格中位数
            for(k = start;k <= end;k ++){
                if(arrb[k] > arrb[median]){
                    break;
                }
            }
            strict = k;
        }
        //如果另一个维度也不满足K匿名
        if(end - strict + 1< K || strict - start < K){
            //那么就开始泛化
            int max,min,astens,aetens,bstens,betens,i,j;
            max = 0;
            min = 100;
            if(flag == 0){//之前是1，现在是0
                //找arrb的最大数和最小数
                for(i = start;i <= end;i ++){
                    if(arrb[i] > max){
                        max =arrb[i];
                    }
                    if(arrb[i] < min){
                        min = arrb[i];
                    }
                }
                //计算暂时LM，最后加入LM
                sub = max - min;
                tLM += (sub / 15.0) * (end - start + 1);
                //开始泛化！
                //tens是十的倍数，前缀a代表arra，前缀b代表arrb，s代表start，e代表end
                astens = arra[start] / 10;
                aetens = arra[end] / 10;
                bstens = min / 10;
                betens = max / 10;
                //泛化
                for(i = start;i <= end;i ++){
                    CAge[writeindex][0] = '[';
                    CAge[writeindex][1] = astens + '0';
                    CAge[writeindex][2] = (arra[start] - astens * 10) + '0';
                    CAge[writeindex][3] = '-';
                    CAge[writeindex][4] = aetens + '0';
                    CAge[writeindex][5] = (arra[end] - aetens * 10) + '0';
                    CAge[writeindex][6] = ']';
                    CEducationNum[writeindex][0] = '[';
                    CEducationNum[writeindex][1] = bstens + '0';
                    CEducationNum[writeindex][2] = (min - bstens * 10) + '0';
                    CEducationNum[writeindex][3] = '-';
                    CEducationNum[writeindex][4] = betens + '0';
                    CEducationNum[writeindex][5] = (max - betens * 10) + '0';
                    CEducationNum[writeindex][6] = ']';
                    writeindex ++;
                }
            }else{
                //之前flag是0，现在是1
                //找arra的最大数和最小数
                for(i = start;i <= end;i ++){
                    if(arra[i] > max){
                        max =arra[i];
                    }   
                    if(arra[i] < min){
                        min = arra[i];
                    }
                }
                //计算暂时LM，最后加入LM
                sub = max - min;
                tLM += (sub / 73.0) * (end - start + 1);
                //开始泛化！
                //tens是十的倍数，前缀a代表arra，前缀b代表arrb，s代表start，e代表end
                astens = min / 10;
                aetens = max / 10;
                bstens = arrb[start] / 10;
                betens = arrb[end] / 10;
                for(i = start;i <= end;i ++){
                    CAge[writeindex][0] = '[';
                    CAge[writeindex][1] = astens + '0';
                    CAge[writeindex][2] = (min - astens * 10) + '0';
                    CAge[writeindex][3] = '-';
                    CAge[writeindex][4] = aetens + '0';
                    CAge[writeindex][5] = (max - aetens * 10) + '0';
                    CAge[writeindex][6] = ']';
                    CEducationNum[writeindex][0] = '[';
                    CEducationNum[writeindex][1] = bstens + '0';
                    CEducationNum[writeindex][2] = (arrb[start] - bstens * 10) + '0';
                    CEducationNum[writeindex][3] = '-';
                    CEducationNum[writeindex][4] = betens + '0';
                    CEducationNum[writeindex][5] = (arrb[end] - betens * 10) + '0';
                    CEducationNum[writeindex][6] = ']';
                    writeindex ++;
                }
            }
            //泛化结束，统计LM
            LM += tLM;
            return;
        }else{
            //如果另一个维度可以继续泛化，那么像另一个维度继续泛化
            Anonymize(arra, arrb, arrc, start, strict - 1, flag);  
            Anonymize(arra, arrb, arrc, strict, end, flag); 
        }
    }else{
        int sel;
        //产生sel随机数
        sel = rand() % 2;
        //printf("%d\n",sel);
        //如果之前flag为0，没必要排序了
        if(sel == 0 && flag == 1){
            QuickSort(arra, arrb, arrc, start, end);
        }else if(sel == 1 && flag == 0){//同理
            QuickSort(arrb, arra, arrc, start, end);
        }
        Anonymize(arra, arrb, arrc, start, strict - 1, sel);
        Anonymize(arra, arrb, arrc, strict, end, sel);
        return;
    }
}
int main(){
    int i,j,min,max;
    clock_t  begin,end;
    begin = clock();
    writeindex = 0;
    Scan();
    //先对Age排序
    QuickSort(Age, EducationNum, occupation, 0, MaxRow - 1);
    //默认初始sel = 0；
    srand(time(0));
    Anonymize(Age, EducationNum, occupation, 0, MaxRow - 1, 0);   
    FILE *fpWrite=fopen("G:\\360MoveData\\Users\\Lenovo\\Desktop\\result_mon.txt","w");
    for(i = 0;i < MaxRow;i ++){
        fprintf(fpWrite, "%s\t%s\t%s\t\n",CAge[i],CEducationNum[i],occupation[i]);
    }
    end = clock();
    printf("cost %dms time\nLM is %f",end - begin,LM/MaxRow);
}