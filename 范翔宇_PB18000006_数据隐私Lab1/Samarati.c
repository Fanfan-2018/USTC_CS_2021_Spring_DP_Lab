#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
int K,MAXSUP;
int age,gender,race,marital;//记录泛化级别
int MaxRow,tmp;//记录数据长度
int besttime;//记录当前最优LM
float bestLM;
int sol[4];//记录结果 
char b[30];
int END;//判断是否有其他可能的向量
int flag[7][4][4][5];
int printflag[33000];
int Age[33000][3];//第一列
char CAge[33000][10];
char Race[33000][20];//第九列
char CRace[33000][20];
char Gender[33000][20];//第十列
char CGender[33000][20];
char Marital[33000][50];//第六列
char CMarital[33000][50]; 
char occupation[33000][20];//第七列
int arrindex[100][10][10][10][1000];
void Scan(){//从data文件中输入，获取数据
    //i用于迭代，scanflag为1时代表此时这行数据是合理的，允许写入，scanflag为0时代表此时这行数据包含"?"，是不合法的
    int i,scanflag;
    int row = 0;
    scanflag = 1;
    FILE *fpRead=fopen("G:\\360MoveData\\Users\\Lenovo\\Desktop\\adult.data","r");
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
            //第一列 Age
            Age[tmp][0] = b[0] - '0';
            Age[tmp][1] = b[1] - '0';
        }else if(row == 6){
            //第六列 Marital
            strcpy(Marital[tmp],b);
        }else if(row == 7){
            //第七列 occupation
            strcpy(occupation[tmp],b);
        }
        else if(row == 9){
            //第九列 Race
            strcpy(Race[tmp],b);
        }else if(row == 10){
            //第十列 Gender
            strcpy(Gender[tmp],b);
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
    fclose(fpRead);
    MaxRow = tmp;
	return;
}
//寻找泛化阶层为n的合法向量
//找到就返回1，否则返回0
int Find(int start, int h){
    int i,j,k,l;
    //没找到好的方法，无脑遍历
    for(i = 0;i < 5;i ++){
        for(j = 0;j < 2;j ++){
            for(k = 0;k < 2;k ++){
                for(l = 0;l < 3;l ++){
                    //当向量元素相加符合条件，且之前未被遍历到
                    if((i + j + k + l) == (start + h) && !flag[i][j][k][l]){
                        age = i;
                        race = j;
                        gender = k;
                        marital = l;
                        flag[i][j][k][l] = 1;
                        return 1;
                    }
                }
            }
        }
    }
    END = 1;
   return 0;
}
//先泛化，再判断是否满足条件
int satisfy(int a,int b,int c,int d){//核心
    //i,j,k,l,m用于迭代，tens、fives泛化时会用到，代表二的倍数、五的倍数，x、y、p、q为泛化阶层对应的下标， 会在count数组中得到应用
    int i,j,k,l,m,tens,fives,x,y,p,q,sum;
    float LM;
    clock_t begin,end;
    //Count数组用于统计泛化后的数据，进而判断是否满足K匿名
    int Count[100][10][10][10];
    //清空评价指标
    LM = 0;
    //清空Count数组和arrindex数组
    //arrindex数组用于记录在泛化到Count[x][y][p][q]中的数据，在最后泛化结果中的具体位置，便于Supress
    for(i = 0;i < 100;i ++){
        for(j = 0;j < 10;j ++){
            for(k = 0;k < 10;k ++){
                for(l = 0;l < 10;l ++){
                    Count[i][j][k][l] = 0;
                    for(m = 0;m < 1000;m ++){
                        arrindex[i][j][k][l][m] = 0;
                    }
                }
            }
        }
    }
    //直接泛化
    begin = clock();
    for(i = 0;i < MaxRow;i ++){
        printflag[i] = 0;//遍历泛化的同时，将printflag清空，这个数组表示对应下标的数据是否被supress
        //处理Age MIN 17 MAX 90
        if(a == 0){
            //a为0的话，保持Age原始数据，将int转换成char即可。
            CAge[i][0] = Age[i][0] + '0';
            CAge[i][1] = Age[i][1] + '0';
            CAge[i][2] = '\0';
            //x即对应Age
            x = Age[i][0] * 10 + Age[i][1];
            LM += 0;
        }else if(a == 1){
            //range 5
            //fives是5的倍数，当fives为0时，个位数对应0-4，为1时对应5-10，十位数保持原样即可
            fives = Age[i][1] / 5;
            CAge[i][0] = '[';
            CAge[i][1] = Age[i][0] + '0';
            CAge[i][2] = (fives * 5) + '0';
            CAge[i][3] = '-';
            CAge[i][4] = Age[i][0] + '0';
            CAge[i][5] = (fives * 5 + 4) + '0';
            CAge[i][6] = ']';
            CAge[i][7] = '\0';
            //x具体为什么值其实不重要，只要可以唯一标识就行
            x = 2 * Age[i][0] + fives;
            LM += 1.0 / 15.0;
        }else if(a == 2){//range 10
            //rang 10
            //这时候只要Age的十位数起到作用，直接转化为char泛化即可
            CAge[i][0] = '[';
            CAge[i][1] = Age[i][0] + '0';
            CAge[i][2] = '0';
            CAge[i][3] = '-';
            CAge[i][4] = Age[i][0] + '0';
            CAge[i][5] = '9';
            CAge[i][6] = ']';
            CAge[i][7] = '\0';
            x = Age[i][0];
            LM += 1.0 / 8.0;
        }else if(a == 3){
            //rang 20
            //tens是二的倍数(整数)，如18，tens为0(1/2 = 0)，那么18在00-19，即泛化的下界为tens
            tens = Age[i][0] / 2;
            CAge[i][0] = '[';
            CAge[i][1] = (tens * 2) + '0';
            CAge[i][2] = '0';
            CAge[i][3] = '-';
            CAge[i][4] = (tens * 2 + 1) + '0';
            CAge[i][5] = '9';
            CAge[i][6] = ']';
            CAge[i][7] = '\0';
            //这时tens可以唯一标识x
            x  = tens;
            LM += 2.0 / 9.0;
        }else if(a == 4){
            //*，全部泛化为"*"，所有数据是一类
            strcpy(CAge[i],"*");
            x = 0;
            LM += 1;
        }
        //处理Race
        if(b == 0){
            //虽然是为原数据，但是要标识区分，但是我们没必要比较全部的数据
            strcpy(CRace[i],Race[i]);
            if(Race[i][0] == 'O'){//只有"Other"的第一位为'O'
                y = 0;
            }else if(Race[i][1] == 'm'){//只有"Amer-Indian-Eskimo"的第二位为'm'
                y = 1;
            }else if(Race[i][0] == 'B'){//只有"Black"的第一位为'B'
                y = 2;
            }else if(Race[i][0] == 'W'){//只有"White"的第一位为'W'
                y = 3;
            }else if(Race[i][1] == 's'){//只要"Asian-Pac-Islander"的第二位为's'
                y = 4;
            }
            LM += 0.0;
        }else if(b == 1){
            //全部泛化为"*"
            strcpy(CRace[i],"*");
            y = 0;
            LM += 1.0;
        }
        //泛化Gender 
        if(c == 0){
            //泛化阶层为0，为原始数据，但要标识区分
            strcpy(CGender[i],Gender[i]);
            p = (Gender[i][0] == 'M') ? 0 : 1;
            LM += 0.0;
        }
        else if(c == 1){
            //全部泛化为"*"
            strcpy(CGender[i],"*");
            p = 0;
            LM += 1.0;
        }
        if(d == 0){
            strcpy(CMarital[i],Marital[i]);
            if(Marital[i][3] == 'e'){//只有"Never-married"的第四位为'e'
                q = 0;
            }else if(Marital[i][8] == 'c'){//只有"Married-civ-spouse"的第九位为'c'
                q = 1;
            }else if(Marital[i][8] == 'A'){//只有"Married-AF-spouse"的第九位为'A'
                q  = 2;
            }else if(Marital[i][0] == 'D') {//只有"Divorced"的第一位为'D'
                q = 3;
            }else if(Marital[i][0] == 'S'){//只有"Separated"的第一位为'S'
                q = 4;
            }else if(Marital[i][0] == 'W'){//只有"Widowed"的第一位为'W'
                q = 5;
            }else if(Marital[i][8] == 's'){//只有"Married-spouse-absent"的第九位为's'
                q = 6;
            }
            LM += 0.0;
        }
        else if(d == 1){
            //怎么标识的不在这里细说了，可以参考d==0
            if(Marital[i][3] == 'e'){
                q = 0;
                strcpy(CMarital[i],"NM");
            }else if(Marital[i][8] == 'c' || Marital[i][8] == 'A'){
                q = 1;
                strcpy(CMarital[i],"Married");
            }else if(Marital[i][0] == 'D' || Marital[i][0] == 'S'){
                q = 2;
                strcpy(CMarital[i],"leave");
            }else if(Marital[i][0] == 'W' || Marital[i][8] == 's'){
                q = 3;
                strcpy(CMarital[i],"alone");
            }
            LM += 1.0/2.0;
        }else if(d == 2){
            //全部泛化为"*"
            strcpy(CMarital[i],"*");
            q = 0;
            LM += 1.0;
        }
        //记录此时对应向量[x,y,p,q]的下标
        arrindex[x][y][p][q][Count[x][y][p][q]] = i;
        //对应向量[x,y,p,q]的数目增加
        Count[x][y][p][q] ++;
    }
    //sum清零，用于记录supress个数
    sum = 0;
    //遍历判断Count中是否存在数目小于K的向量
    for(i = 0;i < 100;i ++){
        for(j = 0;j < 10;j ++){
            for(k = 0;k < 10;k++){
                for(l = 0;l <10;l++){
                    if(Count[i][j][k][l]!=0 && Count[i][j][k][l] < K){
                        //如果小于K，则先判断supress的个数是否大于MAXSUP
                        sum += Count[i][j][k][l];
                        Count[i][j][k][l] = 0;
                        if(sum > MAXSUP){
                            //不符合要求，返回0
                            return 0;
                        }
                    }else if(Count[i][j][k][l] != 0 && Count[i][j][k][l] >= K){
                        //否则，大于K，则满足输出要求，允许输出
                        for(m = 0;m <= Count[i][j][k][l];m ++){
                            printflag[arrindex[i][j][k][l][m]] = 1;
                        }
                    }
                }
            }
        }
    }

    end = clock();
    LM = LM /MaxRow;
    if(LM < bestLM){
        bestLM = LM;
        sol[0] = a;
        sol[1] = b;
        sol[2] = c;
        sol[3] = d;
        besttime = end - begin;
    }else if(LM == bestLM && (end - begin) < besttime){
        bestLM = LM;
        sol[0] = a;
        sol[1] = b;
        sol[2] = c;
        sol[3] = d;
        besttime = end - begin;
    }
    return 1;
} 

int main(){
    int i,low,height,try,reasearch;//中间四个意义同PPT算法
    clock_t begin,end;
    printf("---------------------please input your K and MAXSUP---------------------\n");
    scanf("%d %d",&K,&MAXSUP);
    printf("---------------------please wait---------------------\n");
    begin = clock();//开始时间
    Scan();
    low = 0;height = 8;
    FILE *fpWrite=fopen("G:\\360MoveData\\Users\\Lenovo\\Desktop\\result.txt","w");
    for(i = 0;i < 4;i ++){
        sol[i] = 0;
    }
    //按照PPT算法
    besttime = 1000000;
    bestLM = 100000;
    while(low < height){
        try = (low + height)/2;
        reasearch = 0;
        END = 0;
        while(Find(try,0) && END != 1){
            if(satisfy(age,race,gender,marital) == 1 ){
                //满足条件就记录泛化阶层
                reasearch = 1;
            }
        }
        END = 0;
        //更新high或low
        if(reasearch)   height = try;
        else low = try + 1;
    }
    //因为判断是否满足的时已经泛化了，所以最后还要再泛化到正确结果
    satisfy(sol[0],sol[1],sol[2],sol[3]);
    printf("---------------------finish and output---------------------\n");
    for(i = 0;i < MaxRow;i ++){
        if(printflag[i] == 1){//为0不输出，即Supress
            fprintf(fpWrite,"%s\t%s\t%s\t%s\t%s\t\n", CAge[i],CRace[i],CGender[i],CMarital[i],occupation[i]);  
        }
    }
    end = clock();//整体完成时间
    printf("cost %dms time\nthe best LM is %lf\nAge:%d Race:%d Gender:%d Marital:%d",end - begin,bestLM,sol[0],sol[1],sol[2],sol[3]);
    return 0;
}