#include "card.h"

void shuffle(byte* arr,int len){
	srand((unsigned)time(NULL));
	for(int i=len-1; i>0; --i){
		int p=rand()%(i+1);
		byte tmp=arr[i];
		arr[i]=arr[p];
		arr[p]=tmp;
	}
}

bool is_straight(byte* arr, int len){
	bool result=false;
	int i=len-1;
	do{
		if(i<1)break;
		result=(get_value(arr[i])-get_value(arr[i-1]))==1;
		--i;
	}while(result);

	return result;
}

void analyse_combo(byte* arr,int len, ComboAnalysis& analy){
	ComboAnalysis::Counter counter[len];int counter_len=0;
	for(int i=0,j=0; i<len; ++i){
		do{
			if(j>=counter_len){
				counter[j].val=arr[i];
				counter[j].num=1;
				++counter_len;
				break;
			}
			if((arr[i]&0x0F) == (counter[j].val&0x0F)){
				++counter[j].num;
				break;
			}
			++j;
		}while(1);
		j=0;
	}
	ComboAnalysis tmp_analy={0,0,0,0,{0,0,0,0,false}};
	tmp_analy.count=len;
	tmp_analy.unequal=counter_len;
	tmp_analy.equal_max=len>0? 1:0;
	tmp_analy.equal_min=len;
	for(int i=0; i<counter_len; ++i){
		if(counter[i].num==1)tmp_analy.combo.single++;
		if(counter[i].num==2)tmp_analy.combo.pair++;
		if(counter[i].num==3)tmp_analy.combo.three++;
		if(counter[i].num==4)tmp_analy.combo.four++;
		tmp_analy.equal_max=counter[i].num>=tmp_analy.equal_max? counter[i].num:tmp_analy.equal_max;
		tmp_analy.equal_min=counter[i].num<=tmp_analy.equal_min? counter[i].num:tmp_analy.equal_min;
	}
	byte tmp_arr[len];int tmp_arr_len=0;
	for(int i=0; i<len; ++i){
		if(counter[i].num == tmp_analy.equal_max){
			tmp_arr[tmp_arr_len++]=counter[i].val;
		}
	}
	sort(tmp_arr, tmp_arr_len);
	tmp_analy.combo.straight=is_straight(tmp_arr, tmp_arr_len);
	memcpy(&analy, &tmp_analy, sizeof(ComboAnalysis));
}

/*
len min	max	dif
1	1	1	1	a
2	2	2	1	aa
2	1	1	2	NM
3	3	3	1	aaa
4	1	3	2	aaab
4	4	4	1	aaaa
5	1	1	5	abcde
5	2	3	2	aaabb
6	1	4	3	aaaabc
6	2	2	3	aabbcc
6	3	3	2	aaabbb
8	2	4	3	aaaabbcc
8	1	3	4	aaabbbcd
10	2	3	4	aaabbbccdd
*/

bool match_analyse(ComboAnalysis result, int count, int min, int max, int unequal,bool straight){
	return result.count==count 
		&& result.equal_min==min 
		&& result.equal_max==max 
		&& result.unequal==unequal 
		&& result.combo.straight==straight;
}

int get_type(byte* arr, int len, const ComboAnalysis result){
	if(len==0)return CardType_Check;
	if(match_analyse(result,1,1,1,1,false))return CardType_Single;
	if(match_analyse(result,2,2,2,1,false))return CardType_Pair;
	if(match_analyse(result,2,1,1,2,false))return CardType_Rocket;
	if(match_analyse(result,3,3,3,1,false))return CardType_Three;
	if(match_analyse(result,4,1,3,2,false))return CardType_Three_Attach_Single;
	if(match_analyse(result,4,4,4,1,false))return CardType_Bomb;
	if(match_analyse(result,5,1,1,5,true))return CardType_Straight;
	if(match_analyse(result,5,2,3,2,false))return CardType_Three_Attach_Pair;
	if(match_analyse(result,6,1,4,3,false))return CardType_Four_Attach_Single;
	if(match_analyse(result,6,2,2,3,true))return CardType_Straight_Pair;
	if(match_analyse(result,6,3,3,2,true))return CardType_Straight_Three;
	if(match_analyse(result,8,2,4,3,false))return CardType_Four_Attach_Pair;
	if(match_analyse(result,8,1,3,4,true))return CardType_Straight_Three_Attach_Single;
	if(match_analyse(result,10,2,3,4,true))return CardType_Straight_Three_Attach_Pair;
	return CardType_Check;
}

void match_type(int single, int pair, int three, int four);

int _get_type(const ComboAnalysis* analy){
	if(analy->count==0)return CardType_Check;
	if(analy->count==1)return CardType_Single;
	if(analy->count==2){
		if(analy->equal_max==2)return CardType_Pair;
	}
	return CardType_Check;
}


byte get_value(byte card){
    byte val=card&0x0F;
    val=(card&0xF0)==0x40? val+2:val;
    val=val<=2? val+13:val;
    return val;
}

//O(n^2)
void sort(byte* arr,int len){
  for(int i=1; i<len; ++i){
    if(get_value(arr[i]) < get_value(arr[i-1])){
	int j=i-1;
	byte tmp=arr[i];
	arr[i]=arr[i-1];
	while(get_value(tmp) < get_value(arr[j])){
          arr[j+1]=arr[j];
          --j;
          if(j<0)break;
        }
        arr[j+1]=tmp;
    }
  }
}

bool contains_value(byte one,byte* arr,int len){
  for(int i=0; i<len; ++i){
    if(arr[i] == 0xFF)return false;
    if((arr[i]&0x0F) == (one&0x0F))return true;
  }
  return true;
}