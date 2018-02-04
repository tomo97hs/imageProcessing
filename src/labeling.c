#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define WIDTH 927 /*画像の横サイズ*/
#define HEIGHT 833 /*画像の縦サイズ*/


int getLevel(int P, int hist[]);
int pgmLabeling(int size1, int size2, unsigned char img[size1][size2]);
int bmpLabeling(int size1, int size2, int size3, unsigned char img[size1][size2][size3]);

int main(void)
{
  int histgram[256]={0}; /*ヒストグラム配列*/
  int k, i, j, col;
  int per = 10; /*画像に占める物体の割合(%)*/
  char filename[128], output[128], gray_file[128], ttv_file[128]; /*ファイル名の配列*/
  unsigned char header[54];
  unsigned char bImg_work[HEIGHT][WIDTH][3]; /*bImg_work[][][color(b=0,g=1,r=2と割り当てられる)]*/
  unsigned char bScreen[HEIGHT][WIDTH][3];
  unsigned char pImg_work[HEIGHT][WIDTH];
  unsigned char pScreen[HEIGHT][WIDTH];
  FILE *fp1, *fp2, *fp3, *fp4; /*ファイルポインタ*/

  printf("ファイル名を入れてください.\n>>");
  scanf("%s", filename);

  if(strstr(filename, ".bmp") != NULL){
    if((fp1 = fopen(filename , "rb")) == NULL){
      fprintf(stderr, "ファイルが開けません\n");
      return EXIT_FAILURE;
    }
    fread(header, 1, 54, fp1); /*ヘッダを飛ばす*/
    fread(bScreen, 1, HEIGHT*WIDTH*3,fp1); /*残りはデータ*/
    fclose(fp1);

    /*カラー画像をグレースケール画像に変換する*/
    for(i=0; i<WIDTH; i++){
      for(j=0; j<HEIGHT; j++){
        for(col=0; col<3; col++){
          bImg_work[j][i][col] = 0.1145*bScreen[j][i][0] + 0.5866*bScreen[j][i][1] + 0.2989*bScreen[j][i][2];
        }
      }
    }

    /*グレースケール画像を保存する*/
    printf("グレースケールにする画像ファイル名を入れてください.(拡張子:bmp)\n>>");
    scanf("%s", gray_file);
    if((fp2 = fopen(gray_file, "wb")) == NULL){
      fprintf(stderr, "ファイルが保存できません\n");
      return EXIT_FAILURE;
    }
    fwrite(header, 1, 54, fp2);
    fwrite(bImg_work, 1, HEIGHT*WIDTH*3, fp2);
    fclose(fp2);

    /*ヒストグラム取得*/
    for(i=0; i<WIDTH; i++){
      for(j=0; j<HEIGHT; j++){
        for(k=0; k<256; k++){
          if(k == bImg_work[j][i][0]){
            histgram[k]++;
          }
        }
      }
    }

    printf("出力ファイル名を入れてください\n>>");
    scanf("%s", output);

    /*ヒストグラムファイル書き込み*/
    if((fp3 = fopen(output,"wb"))==NULL){
      fprintf(stderr, "ファイルが保存出来ません\n");
      return EXIT_FAILURE;
    }
    for(i=0;i<256;i++){
      fprintf(fp3,"%d\n",histgram[i]);
    }

    fclose(fp3);

    /*2値化処理を行う*/
    for(i=0; i<WIDTH; i++){
      for(j=0; j<HEIGHT; j++){
        if(bImg_work[j][i][0] >= getLevel(per, histgram)){
          for(col=0; col<3; col++){
            bImg_work[j][i][col] = 255;
          }
        } else{
          for(col=0; col<3; col++){
            bImg_work[j][i][col] = 0;
          }
        }
      }
    }

    /*2値化画像を保存する*/
    printf("2値化する画像ファイル名を入力してください.(拡張子:bmp)\n>>");
    scanf("%s", ttv_file);

    if((fp4=fopen(ttv_file, "wb")) == NULL){
      fprintf(stderr, "ファイルが保存できません\n");
      return EXIT_FAILURE;
    }
    fwrite(header, 1, 54, fp4);
    fwrite(bImg_work, 1, HEIGHT*WIDTH*3, fp4);

    fclose(fp4);

    printf("連結成分の個数 = %d\n", bmpLabeling(HEIGHT, WIDTH, 3, bImg_work));

  } else{
    if((fp1 = fopen(filename , "rb")) == NULL){
      fprintf(stderr, "ファイルが開けません\n");
      return EXIT_FAILURE;
    }
    fread(header, 1, 54, fp1);
    fread(pScreen, 1, HEIGHT*WIDTH,fp1);
    fclose(fp1);

    /*ヒストグラム取得*/
    for(i=0; i<WIDTH; i++){
      for(j=0; j<HEIGHT; j++){
        histgram[pScreen[j][i]]++;
      }
    }

    printf("出力ファイル名を入れてください\n>>");
    scanf("%s", output);

    /*ヒストグラムファイル書き込み*/
    if((fp2 = fopen(output,"wb"))==NULL){
      fprintf(stderr, "ファイルが保存出来ません\n");
      return EXIT_FAILURE;
    }
    for(i=0;i<256;i++){
      fprintf(fp2,"%d\n",histgram[i]);
    }

    fclose(fp2);

    /*2値化処理を行う*/
    for(i=0; i<WIDTH; i++){
      for(j=0; j<HEIGHT; j++){
        if(pScreen[j][i] >= getLevel(per, histgram)){
            pImg_work[j][i] = 255;
        } else{
            pImg_work[j][i] = 0;
        }
      }
    }

    /*2値化画像を保存する*/
    printf("2値化する画像ファイル名を入力してください.(拡張子:bmp)\n>>");
    scanf("%s", ttv_file);

    if((fp3=fopen(ttv_file, "wb")) == NULL){
      fprintf(stderr, "ファイルが開けません\n");
      return EXIT_FAILURE;
    }
    fwrite(header, 1, 54, fp3);
    fwrite(pImg_work, 1, HEIGHT*WIDTH, fp3);

    fclose(fp3);
  }

  printf("閾値：");
  printf("%d\n", getLevel(per, histgram));

  printf("連結成分の個数 = %d\n", pgmLabeling(HEIGHT, WIDTH, pImg_work));

  return EXIT_SUCCESS;
}


/*Pタイル法で閾値を求める*/
int getLevel(int P, int hist[])
{
  int i, n, t;

  t = (int)(HEIGHT*WIDTH*P/100.0);
  n = 0;
  for(i=255; i>=0; i--){
    n += hist[i];
    if(n >= t){
      return i;
    }
  }
  return 0;
}


int pgmLabeling(int size1, int size2, unsigned char img[size1][size2]){
  int i, j, count = 0;
  int label[HEIGHT][WIDTH];

  /*白いますに到着したら*/
  for(i=0; i<WIDTH; i++){
    for(j=0; j<HEIGHT; j++){
        if(img[j][i] == 255){
          /*左上、上、右上、左が黒いマスだったら*/
          if(img[j-1][i-1]==0 && img[j-1][i]==0 && img[j-1][i+1] && img[j][i-1]){
            count++;
            label[j][i] = count;
          } else{ /*左上、上、右上、左のどれかのマスにラベルが振られていたら*/
            /*複数あるうちの最小のラベルの値を割り振る(0以外)*/
            int minLabel = count;
            if(img[j-1][i-1]!=0 && (minLabel > label[j-1][i-1])) minLabel = label[j-1][i-1];
            if(img[j-1][i]!=0 && (minLabel > label[j-1][i])) minLabel = label[j-1][i];
            if(img[j-1][i+1]!=0 && (minLabel > label[j-1][i+1])) minLabel = label[j-1][i+1];
            if(img[j][i-1]!=0 && (minLabel > label[j][i-1])) minLabel = label[j][i-1];

            /*ラベルが振られている隣接マスを最小のものにする*/
            if(img[j-1][i-1]!=0) label[j-1][i-1] = minLabel;
            if(img[j-1][i]!=0) label[j-1][i] = minLabel;
            if(img[j-1][i+1]!=0) label[j-1][i+1] = minLabel;
            if(img[j][i-1]!=0) label[j][i-1] = minLabel;
            label[j][i] = minLabel;
          }
        }
      }
    }
  /*ラベル調整*/
  for(i=WIDTH; i>0; i--){
    for(j=HEIGHT; j>0; j--){
        if(img[j][i] == 255){
          /*左上、上、右上、左が黒いマスだったら*/
          if(img[j+1][i+1]==0 && img[j+1][i]==0 && img[j+1][i-1] && img[j][i+1]){
          }else{ /*左上、上、右上、左のどれかのマスにラベルが振られていたら*/
            /*複数あるうちの最小のラベルの値を割り振る(0以外)*/
            int minLabel = count;
            if(img[j+1][i+1]!=0 && (minLabel > label[j+1][i+1])) minLabel = label[j+1][i+1];
            if(img[j+1][i]!=0 && (minLabel > label[j+1][i])) minLabel = label[j+1][i];
            if(img[j+1][i-1]!=0 && (minLabel > label[j+1][i-1])) minLabel = label[j+1][i-1];
            if(img[j][i+1]!=0 && (minLabel > label[j][i+1])) minLabel = label[j][i+1];

            /*ラベルが振られている隣接マスを最小のものにする*/
            if(img[j+1][i+1]!=0) label[j+1][i+1] = minLabel;
            if(img[j+1][i]!=0) label[j+1][i] = minLabel;
            if(img[j+1][i-1]!=0) label[j+1][i-1] = minLabel;
            if(img[j][i+1]!=0) label[j][i+1] = minLabel;
            label[j][i] = minLabel;
          }
        }
    }
  }

  int maxLabel = 0;

  for(i=0; i<WIDTH; i++){
    for(j=0; j<HEIGHT; j++){
      if(label[j][i] > maxLabel) maxLabel = label[j][i];
    }
  }
  return maxLabel;
}

int bmpLabeling(int size1, int size2, int size3, unsigned char img[size1][size2][size3]){
  int i, j, col, count = 0;
  int label[4000][4000][col];

  /*白いますに到着したら*/
  for(i=0; i<WIDTH; i++){
    for(j=0; j<HEIGHT; j++){
      for(col=0; col<3; col++){
        if(img[j][i][col] == 255){
          /*左上、上、右上、左が黒いマスだったら*/
          if(img[j-1][i-1][col]==0 && img[j-1][i][col]==0 && img[j-1][i+1][col] && img[j][i-1][col]){
            count++;
            label[j][i][col] = count;
          } else{ /*左上、上、右上、左のどれかのマスにラベルが振られていたら*/
            /*複数あるうちの最小のラベルの値を割り振る(0以外)*/
            int minLabel = count;
            if(img[j-1][i-1][col]!=0 && (minLabel > label[j-1][i-1][col])) minLabel = label[j-1][i-1][col];
            if(img[j-1][i][col]!=0 && (minLabel > label[j-1][i][col])) minLabel = label[j-1][i][col];
            if(img[j-1][i+1][col]!=0 && (minLabel > label[j-1][i+1][col])) minLabel = label[j-1][i+1][col];
            if(img[j][i-1][col]!=0 && (minLabel > label[j][i-1][col])) minLabel = label[j][i-1][col];

            /*ラベルが振られている隣接マスを最小のものにする*/
            if(img[j-1][i-1][col]!=0) label[j-1][i-1][col] = minLabel;
            if(img[j-1][i][col]!=0) label[j-1][i-1][col] = minLabel;
            if(img[j-1][i+1][col]!=0) label[j-1][i+1][col] = minLabel;
            if(img[j][i-1][col]!=0) label[j][i-1][col] = minLabel;
            label[j][i][col] = minLabel;
          }
        }
      }
    }
  }

  /*ラベル調整*/
  for(i=WIDTH; i>0; i--){
    for(j=HEIGHT; j>0; j--){
      for(col=0; col<3; col++){
        if(img[j][i][col] == 255){
          /*左上、上、右上、左が黒いマスだったら*/
          if(img[j+1][i+1][col]==0 && img[j+1][i][col]==0 && img[j+1][i-1][col] && img[j][i+1][col]){
          }else{ /*左上、上、右上、左のどれかのマスにラベルが振られていたら*/
            /*複数あるうちの最小のラベルの値を割り振る(0以外)*/
            int minLabel = count;
            if(img[j+1][i+1][col]!=0 && (minLabel > label[j+1][i+1][col])) minLabel = label[j+1][i+1][col];
            if(img[j+1][i][col]!=0 && (minLabel > label[j+1][i][col])) minLabel = label[j+1][i][col];
            if(img[j+1][i-1][col]!=0 && (minLabel > label[j+1][i-1][col])) minLabel = label[j+1][i-1][col];
            if(img[j][i+1][col]!=0 && (minLabel > label[j][i+1][col])) minLabel = label[j][i+1][col];

            /*ラベルが振られている隣接マスを最小のものにする*/
            if(img[j+1][i+1][col]!=0) label[j+1][i+1][col] = minLabel;
            if(img[j+1][i][col]!=0) label[j+1][i][col] = minLabel;
            if(img[j+1][i-1][col]!=0) label[j+1][i-1][col] = minLabel;
            if(img[j][i+1][col]!=0) label[j][i+1][col] = minLabel;
            label[j][i][col] = minLabel;
          }
        }
      }
    }
  }

  int maxLabel = 0;

  for(i=0; i<WIDTH; i++){
    for(j=0; j<HEIGHT; j++){
      for(col=0; col<3; col++){
        if(label[j][i][col] > maxLabel) maxLabel = label[j][i][col];
      }
    }
  }
  return maxLabel;
}
