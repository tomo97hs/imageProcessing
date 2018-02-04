#include<stdio.h>
#include<stdlib.h>

#define WIDTH 108 /*画像の縦サイズ*/
#define HEIGHT 108 /*画像の横サイズ*/

int main(void)
{
  int histgram[256];
  int i, j;
  char filename[40], output[40];
  unsigned char f[HEIGHT][WIDTH];
  FILE *fp1, *fp2; /*ファイルポインタ*/

  printf("ファイル名を入れてください:");
  scanf("%s",filename);

  /*画像ファイル読み込み*/
  if((fp1 = fopen(filename , "rb")) == NULL){
    fprintf(stderr, "ファイルが開けません\n");
    return EXIT_FAILURE;
  }

  fread(f, sizeof(unsigned char), HEIGHT*WIDTH,fp1);
  fclose(fp1);

  /*ヒストグラム配列の初期化*/
  for(i=0; i<256; i++){
    histgram[i] = 0;
  }

  /*ヒストグラム取得*/
  for(i=0; i<HEIGHT; i++){
    for(j=0; j<WIDTH; j++){
      histgram[f[i][j]]++;
    }
  }

  printf("出力ファイル名を入れてください:");
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
  return EXIT_SUCCESS;
}
