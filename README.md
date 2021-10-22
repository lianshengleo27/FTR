<!-- title: 手順書 -->
## 1. バイナリ ファイルのBuild & 実行

### 1.1. シンボリックリンクの構築
---

- In`shared_sample` フォルダ
```bash
# `diffelev` と `shared_object` フォルダへのリンクを構築

ln -s ../shared_object diffelev
```
- `diffelev` folder creared

- Prepare `libxerces-c-3.2.so` Library and add to `diffelev` folder created

### 1.2. ファイルのBuild
---
- In `shared_sample` フォルダ, type:
```
make
```
- バイナリの実行ファイル `sample_diffelev` を生成する

<!-- ![image](https://logowik.com/content/uploads/images/visual-studio-code7642.jpg) -->
<!-- <img src="https://logowik.com/content/uploads/images/visual-studio-code7642.jpg" alt="alt text" width="50" height="20"> -->



![1](https://user-images.githubusercontent.com/56211764/131423087-a1f5da4c-b6e8-4dc9-ace6-893afe2e4c92.png)



### 1.3. ファイルの実行
---

- In current directory where .exe file locates, type:

```
./sample_diffelev <--Option>
```

- 入力オプションの詳細を示す
```
Options:
  -p, --input-pcl       入力点群ファイルを指定します
  -g, --thin-grid       入力点群をグリッド毎に間引きます 計算方法を指定します
                        "maximum":グリッドのZ座標が最大値の点を残します
                        "minimum":グリッドのZ座標が最小値の点を残します
                        "median":グリッドのZ座標が中央値となる点を残します
                        "mode":グリッドのZ座標でヒストグラムを取った際の最瀕値となる点を残します 続けてヒストグラムの間隔を指定してください
                        "none":間引きを行いません
  -c, --cut-polygon     多角形を指定し、点群データからその内側にある点のみを抽出して残りを消去します
                        最初に多角形の点の数を指定し、続けて1点目のX座標、1点目のY座標、2点目以降...と指定します
  -n, --extract-near    設計データから近い点のみを抽出して残りを消去します
  -t, --thinned-pcl     間引かれたデータの出力先を指定します
  -o, --grid-origin     グリッドの基準位置を数値2つスペース区切りで設定します(初期値:0 0)
  -w, --grid-width      グリッドの感覚を数値2つスペース区切りで設定します(初期値:1 1)
  -s, --separator       入力ファイルの区切り文字を設定します(初期値:',')
  -i, --ignore-lines    入力ファイルの先頭何行を無視するかを指定します(初期値:0)
  -x, --input-landxml   設計データとなるLandXmlファイルを指定します
  -l, --output-stl      設計データから得られた表面形状を指定したファイルにSTL形式で出力します
  -d, --diff-elevation  較差を計算し、指定したファイルに計算結果を出力します
                        各グリッドの較差算出結果のcsvファイル、全体の評価結果のjsonファイルの順に指定します
  -v, --standard-value  較差の計算で使用する規格値等のパラメータが書かれたjsonファイルを指定します
  -u, --out-used-value  較差の計算で使用した規格値等のパラメータが書かれたjsonファイルの出力先を指定します
  -a, --slope-angle     法角度を計算し、指定したファイルに計算結果を出力します
  -j, --judge-slope     評価する点群が天面か法面かを判定する計算方法を指定します
                        "force-top":必ず天面として計算します
                        "force-slope":必ず法面として計算します
                        "calculation":近接の設計データを計算で求め、その傾きで天面か法面かを決定します
  -h, --help            このヘルプを表示します
```


## 2. 出力結果

### 2.1. 事前設定
---
> グリッドの基準位置およびサイズの設定 (in m)

```sh
# 点群密度を変更するため、まずグリッドを設置する

./sample_diffelev -o <defalut: 0 0> -w <default: 1 1>
```

> 入力点群ファイルの指定 `(-p)`
```sh
#　指定されるパスから点群ファイルを読込む

./sample_diffelev -p <path/to/point_cloud_file>
```

> 設計データ(LandXML)ファイルの指定
```sh
#　指定されるパスから設計データを読込む

./sample_diffelev -x <path/to/design_data_file>
```


### 2.2. 評価用点群の生成
---

> 範囲指定に応じた点群の切り出し
```sh
# 範囲を指定する多角形の頂点および各頂点の座標を入力する

./sample_diffelev -c <多角形の頂点の数> <1点目のX座標> <1点目のY座標> <2点目以降...>
```  
<!----------------------------------------------------------------------------->

> 間引き方法の選択 & 出力先の指定
```sh
# 各グリッドにおいて点群密度を減らし、代表点のみを抽出する

./sample_diffelev -g <"maximum" | "minimum" | "median" | "mode"> -t <path/to/output_file>
```
<!----------------------------------------------------------------------------->
> 設計データから±x m範囲の点群抽出 (in m)
```sh
# 設計データから大いに離れる異常値を除外する

./sample_diffelev -n <default: 0.1>
```
<!----------------------------------------------------------------------------->

> *入力例*
```sh
# 点群ファイルdata_sample.txtと設計データdesign_data.xmlを読込み
# 各グリッドにおいて「最大値」、そして設計データから±0.1m範囲の点群のみを抽出する
# 出力先は、同じディレクトリの「output.csv」に保存する

./sample_diffelev -p data_sample.txt -x design_data.xml -g "maximum" -n 0.1 -t output.csv
```


### 2.3. 標高較差 & ヒートマップ用データの算出
---
> 出力先およびファイルの形式を指定する
```sh
# 「標高較差」は.csv、「ヒートマップ用データ」は.JSONファイルにそれぞれ保存する
./sample_diffelev -d diffelev.csv heatmap_data.json 
```

> 規格値の指定

* 事前に「**std_value.json**」というファイルを用意する

```sh
./sample_diffelev -p data_sample.txt -u std_value.json

# `-u` オプションの指定だけでは何も計算処理が行われません。
# 他のオプションと組み合わせて使用する
```

\>> `std_value.json`:
```json
{
    "ignore_distance": 0.05, //	法肩・法尻から±5cm以内に存在する計測点を除く
    "slope_average_threshold": 0.08, //法面の較差値の平均値
    "slope_discard_rate": 0.003, //法面の棄却点数の割合
    "slope_maximum_threshold": 0.19, //法面の較差値の最大値
    "slope_minimum_num_in_m2": 1, //法面の1m2 に対するデータ数
    "slope_minimum_threshold": 0.19, //法面の較差値の最小値（絶対値で指定）
    "top_average_threshold": 0.05, //天端の較差値の平均値
    "top_discard_rate": 0.003, //天端の棄却点数の割合
    "top_maximum_threshold": 0.15, //天端の較差値の最大値
    "top_minimum_num_in_m2": 1, //天端の1m2 に対するデータ数
    "top_minimum_threshold": 0.15, //天端の較差値の最小値（絶対値で指定）
    "top_slope_grad_threshold": 0.06 //平場・法面を判断する基準（三角形の勾配＞0.06 → 法面とする）
}
```


> 入力例
- 規格値を指定する「std_value.json」ファイルをオプションとして入力
- 点群ファイルのseparatorちゃんと確認、`space`の場合は　`-s " "`が必要となる
```sh
./sample_diffelev -p data_sample.txt -x design_data.xml -g "maximum" -d diffelev.csv heatmap_data.json -v std_value.json -s " "
```
> 結果

\>> `heatmap_data.json`:

```json
{
    "area": 305.0, //グリッド点数×グリッド面積
    "average": -4.245125128772781, //平均値：「評価用（間引き後）点群データ標高と設計値」の差の平均値
    "discard": 301, //規格値を外れたデータ個数
    "maximum": 3.0552100422057435, //「評価用（間引き後）点群データ標高と設計値」の差の最大値
    "minimum": -21.354345055558667, //「評価用（間引き後）点群データ標高と設計値」の差の最小値
    "number": 305 //全評価用データ個数（棄却点を含む）
}

```
>> 棄却点数の割合 < 規格値:
>>  最大値、最小値、平均値には棄却点を除いて算出された値が記載されます。
>> 
>> 棄却点数の割合 > 規格値:
>>  最大値、最小値、平均値には棄却点を含めて算出された値が記載されます。

\>> `diffelev.csv`:

| 評価用データ座標Ｘ | 評価用データ座標Ｙ |  設計高  | 現況高  |  標高差  | 規格値比(%) |
| :----------------: | :----------------: | :------: | :-----: | :------: | :---------: |
|       -67082       |      -69394.6      | 234.827  | 236.322 | -0.00996 |  5.242138   |
|      -67082.6      |       -69395       | 235.7194 | 236.32  | -0.11943 |  62.85767   |
|      -67083.3      |      -69394.9      | 235.4363 | 236.301 | -0.00935 |  4.919924   |
|      -67082.9      |      -69395.4      | 234.5186 | 237.111 | 0.005401 |  2.842852   |
|      -67083.3      |      -69395.7      | 234.6022 | 237.27  | 0.003828 |  2.014805   |
|      -67085.5      |       -69394       | 236.4242 | 235.507 | -0.02623 |  13.80531   |
|      -67086.1      |       -69394       | 237.2012 | 235.42  | -0.03015 |  15.86919   |
|        ...         |        ...         |   ...    |   ...   |   ...    |     ...     |



### 2.4. 法角度の算出
---
> 天端か法面かを判定する計算方法を指定する
```sh
 ./sample_diffelev -j <"calculation" | "force-top" | "force-slope">

# "force-top":天端として計算します
# "force-slope":法面として計算します
# "calculation":設計データによる設計面の傾斜角度により、天端か法面かを判定する
```
> 出力先とファイルの形式を指定する
```
./sample_diffelev -a angle.json
```

> 入力例

```sh
./sample_diffelev -p data_sample.txt -x design_data.xml -g "maximum" -a angle.json -j "calculation"
```

> 結果（法角度を示す部分）


```json
//法面
           "approx": {
            "angle": 33.32020836249262,
            "center": [
                -66798.0972314638,
                -69252.52936447694,
                283.926281506519
            ],
            "normal": [
                -0.05375152508800869,
                -0.5466814172140991,
                0.8356136676859093
            ],
            "outline": [    //どれの法面を判断できる
                [
                    -66805.0035541121,
                    -69258.69259499566,
                    279.4498719953511
                ],
                [
                    -66791.6950596824,
                    -69259.0847977335,
                    280.04936185895457
                ],
                [
                    -66791.6950596824,
                    -69247.34066953028,
                    287.73269316187833
                ],
                [
                    -66805.0035541121,
                    -69246.94846679244,
                    287.1332032982748
                ]
            ],
            "valid": "yes"
        },
        "design": {
            "normal": [
                -0.04730468731800854,
                -0.5526830910767048,
                0.8320478756632002
            ],
            "outline": [     //設計データの三角形グループの外形線を出力します。
                [
                    [
                        -67283.77392894664,
                        -69220.1922640378,
                        278.219000000017
                    ],
                    
                    [
                         ....
                    ],
                    
                    [
                        -67282.35887399194,
                        -69199.19789852048,
                        292.247000000017
                    ]
                ]
            ],
            "type": "slope" //平場または法面を示す
            ],
        }
    },
```

### 2.5. STLの出力オプション
```sh
./sample_diffelev -p ixs_data_eval.txt -x ixs_design.xml -l output.stl
```
> -xと-lだけではSTLの出力ができないため、まず他に何かのオプションを入力必要


### 2.6. E57 fileの読込み機能を追加
>　.txt点群ファイルと同じ扱い
>　
```sh
./sample_diffelev -p ixs_data_eval.e57 -x ixs_design.xml -d diffelev.csv heatmap.json -a angle.json -g maximum
```
