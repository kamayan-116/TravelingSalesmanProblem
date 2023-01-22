#define MAX_N 10000   // 点の数の最大値
#define INF 100000000 // 無限大の定義
#define EPSILON 0.00000001 // 小さい正の値
#define SWAP(a,b){int temp; temp=(a); (a)=(b); (b)=temp; }

int num=2;
char tourFileName[20];

struct point {
    int x;
    int y;
};

// pとq の間の距離を計算 
double dist(struct point p, struct point q) {
    return sqrt((p.x-q.x)*(p.x-q.x)+(p.y-q.y)*(p.y-q.y));
}

double tour_length(struct point p[MAX_N], int n, int tour[MAX_N]) {
    int i;
    double sum=0.0;
    for(i=0;i<n;i++) sum+=dist(p[tour[i]],p[tour[(i+1)%n]]);
    return sum;// 総距離が関数の戻り値
}

void read_tsp_data(char *filename, struct point p[MAX_N],int *np, int prec[MAX_N], int *mp) {
    FILE *fp;
    char buff[500];
    int i;

    if ((fp=fopen(filename,"rt")) == NULL) {// 指定ファイルを読み込み用に開く
        fprintf(stderr,"Error: File %s open failed.\n",filename);
        exit(EXIT_FAILURE);
    }   

    while((fgets(buff,sizeof(buff),fp)!=NULL)   // PRECEDENCE_CONSTRAINTS:で始まる行に出会う
        &&(strncmp("PRECEDENCE_CONSTRAINTS:",buff,23)!=0)) ; // まで読み飛ばす. 
    if(strncmp("PRECEDENCE_CONSTRAINTS:",buff,23)==0)  {
        sscanf(buff+24,"%d",mp);
        for(i=0;i<*mp;i++) fscanf(fp,"%d ",&prec[i]);
    } else {
        fprintf(stderr,"Error: There is no precedence constraint in file %s.\n",filename);
        exit(EXIT_FAILURE);
    }

    while((fgets(buff,sizeof(buff),fp)!=NULL)   // DIMENSION で始まる行に出会う
        &&(strncmp("DIMENSION",buff,9)!=0)) ; // まで読み飛ばす. 
    sscanf(buff,"DIMENSION: %d",np);           // 点の数 *np を読み込む

    while((fgets(buff,sizeof(buff),fp)!=NULL)   // NODE_COORD_SECTIONで始まる
        &&(strncmp("NODE_COORD_SECTION",buff,18)!=0)) ; // 行に出会うまで読み飛ばす. 

    for(i=0;i<*np;i++) {                       // i=0 から i=(*np)-1まで
        if(fgets(buff,sizeof(buff),fp)!=NULL) 
        sscanf(buff,"%*d %d %d",&(p[i].x),&(p[i].y)); // i番目の点の座標を読み込む
    }                                 

    fclose(fp);
}

void write_tour_data(char *filename, int n, int tour[MAX_N]){
    FILE *fp; 
    int i;
    
    // 構築した巡回路をfilenameという名前のファイルに書き出すためにopen
    if((fp=fopen(filename,"wt"))==NULL){ 
        fprintf(stderr,"Error: File %s open failed.\n",filename);
        exit(EXIT_FAILURE);
    }
    fprintf(fp,"%d\n",n);
    for(i=0;i<n; i++){
    fprintf(fp,"%d ",tour[i]);
    }
    fprintf(fp,"\n");
    fclose(fp);
}

void insert(int tour[MAX_N], int* len, int k, int value) {
    int i;
    
    if(k<0 || k > *len) {
        printf("Error in insert: out of range\n");
    }
    
    for(i=*len;i>k;i--) {
        tour[i]=tour[i-1];
    }
    tour[k]=value;
    (*len)++;
}

void ci_if(struct point p[MAX_N],int n,int tour[MAX_N],int m,int prec[MAX_N]){
	int i,j,k,a,b,c,r;
	int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
	double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
	int nearest[MAX_N]; /* 未訪問点 r を現在の部分巡回路内の枝(i,i+1)に挿入する
                        ときに最も距離の増加が小さい i を nearest[r]に保存*/
	double min_dist, min_d;
	int min_r;
	int sbtlen=0;

	for(a=0;a<n;a++) visited[a]=0; // 最初は全都市は未訪問
	for(k=0;k<m;k++) visited[prec[k]] = 1;
	for(k=0;k<m;k++) tour[k] = prec[k];
	sbtlen = m;
	
	while(sbtlen < n) {
		min_dist=INF;
		for(r=0;r<n;r++) {
			if(!visited[r]) {
				d[r] = INF;
				for(i=0;i<sbtlen;i++) {
					a=tour[i];
					j=(i+1)%sbtlen; // j== i+1 ただし i=sbtlen-1 のときは, j==0
					if (tour[j] != tour[sbtlen]) {
	     				b = tour[j];
	   			} else {
	     				b = tour[0];
	  				}
					min_d = dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b]);
					if (min_d <d[r]) {
						nearest[r]=i;
						d[r]=min_d;
					}
				}
				if (d[r] < min_dist) {
					min_dist = d[r];
					min_r=r;
				}
			}
		}
		r=min_r;
		i=nearest[r];
		j=(i+1)%sbtlen;
		insert(tour,&sbtlen,j,r); 
		visited[r]=1;
	}
}

void ci(struct point p[MAX_N],int n,int tour[MAX_N],int m,int prec[MAX_N]){
	int i,j,k,a,b,c,r;
	int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
	double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
	int nearest[MAX_N]; /* 未訪問点 r を現在の部分巡回路内の枝(i,i+1)に挿入する
                        ときに最も距離の増加が小さい i を nearest[r]に保存*/
	double min_dist, min_d;
	int min_r;
	int sbtlen=0;

	for(a=0;a<n;a++) visited[a]=0; // 最初は全都市は未訪問
	for(k=0;k<m;k++) visited[prec[k]] = 1;
	for(k=0;k<m;k++) tour[k] = prec[k];
	sbtlen = m;
	
	while(sbtlen < n) {
		min_dist=INF;
		for(r=0;r<n;r++) {
			if(!visited[r]) {
				d[r] = INF;
				for(i=0;i<sbtlen;i++) {
					a=tour[i];
					j=(i+1)%sbtlen; // j== i+1 ただし i=sbtlen-1 のときは, j==0
	     			b = tour[j];
					min_d = dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b]);
					if (min_d <d[r]) {
						nearest[r]=i;
						d[r]=min_d;
					}
				}
				if (d[r] < min_dist) {
					min_dist = d[r];
					min_r=r;
				}
			}
		}
		r=min_r;
		i=nearest[r];
		j=(i+1)%sbtlen;
		insert(tour,&sbtlen,j,r); 
		visited[r]=1;
	}
}

void ci_if_loop(struct point p[MAX_N],int n,int tour[MAX_N],int m,int prec[MAX_N],int ran){
	int h,i,j,k,l,a,b,c,r;
	int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
	double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
	int nearest[MAX_N]; /* 未訪問点 r を現在の部分巡回路内の枝(i,i+1)に挿入する
                        ときに最も距離の増加が小さい i を nearest[r]に保存*/
	double min_dist[MAX_N];
	double min_d;
	int min_r[MAX_N];
	int sbtlen=0, r_count;

	for(a=0;a<n;a++) {
		visited[a]=0; // 最初は全都市は未訪問
		tour[a]=0; // tour を初期化
	}
	for(k=0;k<m;k++) visited[prec[k]] = 1;
	for(k=0;k<m;k++) tour[k] = prec[k];
	sbtlen = m;

	while(sbtlen < n) {
		min_dist[0] = INF;
		r_count = 0;
		for(r=0;r<n;r++) {
			if(!visited[r]) {
				d[r] = INF;
				for(i=0;i<sbtlen;i++) {
					a=tour[i];
					j=(i+1)%sbtlen; // j== i+1 ただし i=sbtlen-1 のときは, j==0
	     			if (tour[j] != tour[sbtlen]) {
	     				b = tour[j];
	   			} else {
	     				b = tour[0];
	  				}
					min_d = dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b]);
					if (min_d <d[r]) {
						nearest[r]=i;
						d[r]=min_d;
					}
				}
				for(h=0; h<=r_count; h++) {
					if (d[r] < min_dist[h]) {
						for(l=r_count; l>h; l--) min_dist[l] = min_dist[l-1];
						min_dist[h] = d[r];
						for(l=r_count; l>h; l--) min_r[l] = min_r[l-1];
						min_r[h] = r;
						r_count++;
						break;
					}
					if(h == r_count) {
						min_dist[h] = d[r];
						min_r[h] = r;
						r_count++;
						break;
					}
				}
			}
		}
		if(n-sbtlen >= ran) {
			r=min_r[ran-1];
			i=nearest[r];
			j=(i+1)%sbtlen;
		} else {
			r=min_r[0];
			i=nearest[r];
			j=(i+1)%sbtlen;
		}
		insert(tour,&sbtlen,j,r); 
		visited[r]=1;
	}
}

void ci_loop(struct point p[MAX_N],int n,int tour[MAX_N],int m,int prec[MAX_N],int ran){
	int h,i,j,k,l,a,b,c,r;
	int visited[MAX_N]; // 都市iが訪問済みならば1そうでなければ0
	double d[MAX_N]; // 未訪問点 r から現在の部分巡回路までの最短距離を d[r] に保存
	int nearest[MAX_N]; /* 未訪問点 r を現在の部分巡回路内の枝(i,i+1)に挿入する
                        ときに最も距離の増加が小さい i を nearest[r]に保存*/
	double min_dist[MAX_N];
	double min_d;
	int min_r[MAX_N];
	int sbtlen=0, r_count;

	for(a=0;a<n;a++) {
		visited[a]=0; // 最初は全都市は未訪問
		tour[a]=0; // tour を初期化
	}
	for(k=0;k<m;k++) visited[prec[k]] = 1;
	for(k=0;k<m;k++) tour[k] = prec[k];
	sbtlen = m;

	while(sbtlen < n) {
		min_dist[0] = INF;
		r_count = 0;
		for(r=0;r<n;r++) {
			if(!visited[r]) {
				d[r] = INF;
				for(i=0;i<sbtlen;i++) {
					a=tour[i];
					j=(i+1)%sbtlen; // j== i+1 ただし i=sbtlen-1 のときは, j==0
	     			b = tour[j];
					min_d = dist(p[a],p[r])+dist(p[b],p[r])-dist(p[a],p[b]);
					if (min_d <d[r]) {
						nearest[r]=i;
						d[r]=min_d;
					}
				}
				for(h=0; h<=r_count; h++) {
					if (d[r] < min_dist[h]) {
						for(l=r_count; l>h; l--) min_dist[l] = min_dist[l-1];
						min_dist[h] = d[r];
						for(l=r_count; l>h; l--) min_r[l] = min_r[l-1];
						min_r[h] = r;
						r_count++;
						break;
					}
					if(h == r_count) {
						min_dist[h] = d[r];
						min_r[h] = r;
						r_count++;
						break;
					}
				}
			}
		}
		if(n-sbtlen >= ran) {
			r=min_r[ran-1];
			i=nearest[r];
			j=(i+1)%sbtlen;
		} else {
			r=min_r[0];
			i=nearest[r];
			j=(i+1)%sbtlen;
		}
		insert(tour,&sbtlen,j,r); 
		visited[r]=1;
	}
}

void TwoOpt(struct point p[MAX_N],int n,int tour[MAX_N],int m,int prec[MAX_N]){
	int a,b,c,d;
	int prec_amount;
	int i,j,k,l,g,h,success,x;
	double max, dif;
	success = 1;
	while(success == 1) {
		success = 0;
		max = 0;
		for(i=0; i<=n-3; i++) {
			j = i+1;
			for(k=i+2; k<=n-1; k++) {
				prec_amount = 0;
				l = (k+1)%n;
				a = tour[i]; b = tour[j];
				c = tour[k]; d = tour[l];
				for(x=0; x<m; x++) {
					for(int z=j; z<=k; z++) {
						if(prec[x] == tour[z]) prec_amount++;
					}
				}
				if(prec_amount > 1) break;
				dif = (dist(p[a], p[b]) + dist(p[c], p[d])) - (dist(p[a], p[c]) + dist(p[b], p[d]));
				if(dif > max) {
					success = 1;
					max = dif;
					g=j; h=k;
					while(g<h) {
						SWAP(tour[g], tour[h]);
						g++; h--;
					}
				}
			}
		}
	}
}

void OrOpt(struct point p[MAX_N],int n,int tour[MAX_N],int m,int prec[MAX_N],int *flag1){
   int h, i, j, k, l, o, q;
	int a, b, c, d, e;
	int count = 1, total = 0, out;
	while(count == 1) {
		count = 0;
		for(i=0; i<n; i++) {
			h = i-1;
			j = i+1;
			if(h < 0) h = n-1;
			if(j == n) j = 0;
			for(k=0; k<n; k++) {
				l = k+1;
				out = 0;
				if(l == n) l = 0;
				if(k != i && l != i) {
					a = tour[h];
					b = tour[i];
					c = tour[j];
					d = tour[k];
					e = tour[l];
					for(int x=0; x<m; x++) {
						if (prec[x] == b) out = 1;
					}
					if(out == 0) {
						if(dist(p[a], p[b]) + dist(p[b], p[c]) + dist(p[d], p[e]) > dist(p[a], p[c]) + dist(p[d], p[b]) + dist(p[b], p[e])) {
							if(i < l) {
	              			for(o=i; o<l; o++) tour[o] = tour[o+1];
      	        			tour[k] = b;
      	      		} else {
      	        			for(q=i; q>l; q--) tour[q] = tour[q-1];
      	        			tour[l] = b;
      	        		}
							count++;
						}
					}
				}
			}
		}
		total += count;
		if(total == 0) *flag1 = 0;
	}
}

void OrOpt2(struct point p[MAX_N],int n,int tour[MAX_N],int m,int prec[MAX_N],int *flag2){
   int h, i, j, k, l, o, q, r;
	int a, b, c, d, e, f;
	int count = 1, total = 0, out;
	while(count == 1) {
		count = 0;
		for(i=0; i<n-1; i++) {
			h = i-1;
			j = i+1;
			k = i+2;
			if(h < 0) h = n-1;
			if(k == n) k = 0;
			for(l=0; l<n-1; l++) {
				o = l+1;
				out = 0;
				if(o == n) o = 0;
				if(l != i && o != i && l != j && o != j) {
					a = tour[h];
					b = tour[i];
					c = tour[j];
					d = tour[k];
					e = tour[l];
					f = tour[o];
					for(int x=0; x<m; x++) {
						if (prec[x] == b || prec[x] == c) out = 1;
					}
					if(out == 0) {
						if(dist(p[a], p[b]) + dist(p[b], p[c]) + dist(p[c], p[d]) + dist(p[e], p[f]) > dist(p[a], p[d]) + dist(p[e], p[b]) + dist(p[b], p[c]) + dist(p[c], p[f])) {
							if(j < o) {
	              			for(q=i; q<l; q++) tour[q] = tour[q+2];
      	        			tour[l-1] = b;
								tour[l] = c;
								count++;
      	      		} else if(o < i) {
      	        			for(r=j; r>o; r--) tour[r] = tour[r-2];
      	        			tour[o] = b;
								tour[o+1] = c;
								count++;
      	        		}
						}
					}
				}
			}
		}
		total += count;
		if(total == 0) *flag2 = 0;
	}
}

void OrOpt3(struct point p[MAX_N],int n,int tour[MAX_N],int m,int prec[MAX_N],int *flag3){
   int h, i, j, k, l, o, q, r, t;
	int a, b, c, d, e, f, g;
	int count = 1, total = 0, out;
	while(count == 1) {
		count = 0;
		for(i=0; i<n-2; i++) {
			h = i-1;
			j = i+1;
			k = i+2;
			l = i+3;
			if(h < 0) h = n-1;
			if(l == n) l = 0;
			for(o=0; o<n-1; o++) {
				q = o+1;
				out = 0;
				if(q == n) q = 0;
				if(o != i && q != i && o != j && q != j && o != k && q != k) {
					a = tour[h];
					b = tour[i];
					c = tour[j];
					d = tour[k];
					e = tour[l];
					f = tour[o];
					g = tour[q];
					for(int x=0; x<m; x++) {
						if (prec[x] == b || prec[x] == c || prec[x] == d) out = 1;
					}
					if(out == 0) {
						if(dist(p[a], p[b]) + dist(p[b], p[c]) + dist(p[c], p[d]) + dist(p[d], p[e]) + dist(p[f], p[g]) > dist(p[a], p[e]) + dist(p[f], p[b]) + dist(p[b], p[c]) + dist(p[c], p[d]) + dist(p[d], p[g])) {
							if(k < q) {
	              			for(r=i; r<o-2; r++) tour[r] = tour[r+3];
								tour[o-2] = b;
      	        			tour[o-1] = c;
								tour[o] = d;
								count++;
      	      		} else if(q < i) {
      	        			for(t=k; t>q+2; t--) tour[t] = tour[t-3];
      	        			tour[q] = b;
								tour[q+1] = c;
								tour[q+2] = d;
								count++;
      	        		}
						}
					}
				}
			}
		}
		total += count;
		if(total == 0) *flag3 = 0;
	}
}

void OrOpt4(struct point p[MAX_N],int n,int tour[MAX_N],int m,int prec[MAX_N],int *flag4){
   int i, j, k, l, o, q, r, s, t, u;
	int a, b, c, d, e, f, g, h;
	int count = 1, total = 0, out;
	while(count == 1) {
		count = 0;
		for(j=0; j<n-3; j++) {
			i = j-1;
			k = j+1;
			l = j+2;
			o = j+3;
			q = j+4;
			if(i < 0) i = n-1;
			if(q == n) q = 0;
			for(r=0; r<n-1; r++) {
				s = r+1;
				out = 0;
				if(s == n) s = 0;
				if(r != j && s != j && r != k && s != k && r != l && s != l && r != o && s != o) {
					a = tour[i];
					b = tour[j];
					c = tour[k];
					d = tour[l];
					e = tour[o];
					f = tour[q];
					g = tour[r];
					h = tour[s];
					for(int x=0; x<m; x++) {
						if (prec[x] == b || prec[x] == c || prec[x] == d || prec[x] == e) out = 1;
					}
					if(out == 0) {
						if(dist(p[a], p[b]) + dist(p[b], p[c]) + dist(p[c], p[d]) + dist(p[d], p[e]) + dist(p[e], p[f]) + dist(p[g], p[h]) > dist(p[a], p[f]) + dist(p[g], p[b]) + dist(p[b], p[c]) + dist(p[c], p[d]) + dist(p[d], p[e]) + dist(p[e], p[h])) {
							if(o < s) {
	              			for(t=j; t<r-3; t++) tour[t] = tour[t+4];
								tour[r-3] = b;
								tour[r-2] = c;
      	        			tour[r-1] = d;
								tour[r] = e;
								count++;
      	      		} else if(s < j) {
      	        			for(u=o; u>s+3; u--) tour[u] = tour[u-4];
      	        			tour[s] = b;
								tour[s+1] = c;
								tour[s+2] = d;
								tour[s+3] = e;
								count++;
      	        		}
						}
					}
				}
			}
		}
		total += count;
		if(total == 0) *flag4 = 0;
	}
}

int main(int argc, char *argv[]) {
    int n;                   // 点の数 
    int m;                   // 順序制約に現れる点の数
    struct point p[MAX_N];   // 各点の座標を表す配列 
    int tour[MAX_N];   // 巡回路を表現する配列
    int MinTour[MAX_N];   // 最小巡回路を表現する配列
    int prec[MAX_N];   // 順序制約を表現する配列
    int i;
    int ran; // 乱数
    int flag11 = 1, flag12 = 1, flag13 = 1, flag14 = 1; // if文ありの改善法ループ
    int flag21 = 1, flag22 = 1, flag23 = 1, flag24 = 1; // if文なしの改善法ループ
    double MinLength = INF;

    if(argc != 2) {
        fprintf(stderr,"Usage: %s <tsp_filename>\n",argv[0]);
        exit(EXIT_FAILURE);
    }

    // 点の数と各点の座標を1番目のコマンドライン引数で指定されたファイルから読み込む
    read_tsp_data(argv[1],p,&n,prec,&m);

    // if文ありの構築法
    // 最近近傍法による巡回路構築
    ci_if(p,n,tour,m,prec);
    // 巡回路をテキストファイルとして出力
    write_tour_data("tour11.dat",n,tour);
    // 巡回路長を画面に出力
    printf("ci_if = %5.1lf\n",tour_length(p,n,tour)); // 巡回路tour1の長さ
    // if文ありの改善法
    while(flag11 == 1 || flag12 == 1 || flag13 == 1 || flag14 == 1) {
        // 2opt による改善
        TwoOpt(p,n,tour,m,prec);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour12.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tour1の長さ
        // Or-Opt による改善
        OrOpt(p,n,tour,m,prec,&flag11);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour13.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tour1の長さ
        // Or-Opt2 による改善
        OrOpt2(p,n,tour,m,prec,&flag12);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour14.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tour1の長さ
        // Or-Opt3 による改善
        OrOpt3(p,n,tour,m,prec,&flag13);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour15.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tour1の長さ
        // Or-Opt4 による改善
        OrOpt4(p,n,tour,m,prec,&flag14);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour16.dat",n,tour);
        // 巡回路長を画面に出力
        printf("or4_if = %5.1lf\n",tour_length(p,n,tour)); // 巡回路tour1の長さ
    }
    if(tour_length(p,n,tour) < MinLength) {
        MinLength = tour_length(p,n,tour);
        for(i=0; i<n; i++) MinTour[i] = tour[i];
    }
    // 巡回路をテキストファイルとして出力
    write_tour_data("MinTour.dat",n,MinTour);
    // 巡回路長を画面に出力
    printf("MinTour = %5.1lf\n",tour_length(p,n,MinTour)); // 最小巡回路MinTourの長さ
    // if文なしの構築法
    // 最近近傍法による巡回路構築
    ci(p,n,tour,m,prec);
    // 巡回路をテキストファイルとして出力
    write_tour_data("tour21.dat",n,tour);
    // 巡回路長を画面に出力
    printf("ci = %5.1lf\n",tour_length(p,n,tour)); // 巡回路tour2の長さ
    // if文なしの改善法
    while(flag21 == 1 || flag22 == 1 || flag23 == 1 || flag24 ==1) {
        // 2opt による改善
        TwoOpt(p,n,tour,m,prec);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour22.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tour2の長さ
        // Or-Opt による改善
        OrOpt(p,n,tour,m,prec,&flag21);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour23.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tour2の長さ
        // Or-Opt2 による改善
        OrOpt2(p,n,tour,m,prec,&flag22);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour24.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tour2の長さ
        // Or-Opt3 による改善
        OrOpt3(p,n,tour,m,prec,&flag23);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour25.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tour2の長さ
        // Or-Opt4 による改善
        OrOpt4(p,n,tour,m,prec,&flag24);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour26.dat",n,tour);
        // 巡回路長を画面に出力
        printf("or4 = %5.1lf\n",tour_length(p,n,tour)); // 巡回路tour2の長さ
    }
    if(tour_length(p,n,tour) < MinLength) {
        MinLength = tour_length(p,n,tour);
        for(i=0; i<n; i++) MinTour[i] = tour[i];
    }
    // 巡回路をテキストファイルとして出力
    write_tour_data("MinTour.dat",n,MinTour);
    // 巡回路長を画面に出力
    printf("MinTour = %5.1lf\n",tour_length(p,n,MinTour)); // 最小巡回路MinTourの長さ

    // 無限ループ
    while(1) {
        if(n < 1500) {
        ran = rand() % 30 + 1;
        } else {
            ran = rand() % 10 + 1;
        while(ran == 1) ran = rand() % 10 + 1;
        }
        // if文あり
        // 構築法
        // 最近近傍法による巡回路構築
        ci_if_loop(p,n,tour,m,prec,ran);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour11.dat",n,tour);
        // 巡回路長を画面に出力
        printf("ci_if = %5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        flag11 = 1;
        flag12 = 1;
        flag13 = 1;
        flag14 = 1;
        // 改善法
        while(flag11 == 1 || flag12 == 1 || flag13 == 1 || flag14 == 1) {
        // 2opt による改善
        TwoOpt(p,n,tour,m,prec);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour12.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        // Or-Opt による改善
        OrOpt(p,n,tour,m,prec,&flag11);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour13.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        // Or-Opt2 による改善
        OrOpt2(p,n,tour,m,prec,&flag12);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour14.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        // Or-Opt3 による改善
        OrOpt3(p,n,tour,m,prec,&flag13);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour15.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        // Or-Opt4 による改善
        OrOpt4(p,n,tour,m,prec,&flag14);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour16.dat",n,tour);
        // 巡回路長を画面に出力
        printf("or4_if = %5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        }
        if(tour_length(p,n,tour) < MinLength) {
        MinLength = tour_length(p,n,tour);
        for(i=0; i<n; i++) MinTour[i] = tour[i];
        }
        // 巡回路をテキストファイルとして出力
        write_tour_data("MinTour.dat",n,MinTour);
        // 巡回路長を画面に出力
        printf("MinTour = %5.1lf\n",tour_length(p,n,MinTour)); // 最小巡回路MinTourの長さ
        // if文なし
        // 構築法
        // 最近近傍法による巡回路構築
        ci_loop(p,n,tour,m,prec,ran);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour21.dat",n,tour);
        // 巡回路長を画面に出力
        printf("ci = %5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        flag21 = 1;
        flag22 = 1;
        flag23 = 1;
        flag24 = 1;
        // 改善法
        while(flag21 == 1 || flag22 == 1 || flag23 == 1 || flag24 == 1) {
        // 2opt による改善
        TwoOpt(p,n,tour,m,prec);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour22.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        // Or-Opt による改善
        OrOpt(p,n,tour,m,prec,&flag21);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour23.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        // Or-Opt2 による改善
        OrOpt2(p,n,tour,m,prec,&flag22);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour24.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        // Or-Opt3 による改善
        OrOpt3(p,n,tour,m,prec,&flag23);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour25.dat",n,tour);
        // 巡回路長を画面に出力
        printf("%5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        // Or-Opt4 による改善
        OrOpt4(p,n,tour,m,prec,&flag24);
        // 巡回路をテキストファイルとして出力
        write_tour_data("tour26.dat",n,tour);
        // 巡回路長を画面に出力
        printf("or4 = %5.1lf\n",tour_length(p,n,tour)); // 巡回路tourの長さ
        }
        if(tour_length(p,n,tour) < MinLength) {
        MinLength = tour_length(p,n,tour);
        for(i=0; i<n; i++) MinTour[i] = tour[i];
        }
        // 巡回路をテキストファイルとして出力
        write_tour_data("MinTour.dat",n,MinTour);
        // 巡回路長を画面に出力
        printf("MinTour = %5.1lf\n",tour_length(p,n,MinTour)); // 最小巡回路MinTourの長さ
    }
    exit(EXIT_SUCCESS);
}
