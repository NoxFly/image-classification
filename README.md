# Image classification

Tried to implement a small image analyzer which trains then sorts images depending categories.<br>
On this branch, the tests are for parking places. Because it is for implementation and testing purposes, the `Test` folder separates the `Free` and `Full` places.

All IDE configuration files are exclude from the git, so you can setup the project as you want, on Linux and Windows.

## How to run

You have to run from the root directory of this project.<br>
Ensure to have linked OpenCV 4.8, and added dlls if you're on Windows.

## Execution results

Setup used :
- i5-10400F 2.90 GHz (12 cores)
- 32 Go 2666 MHz
- Windows 10
- Visual Studio 2022 17.7.4
- OpenCV 4.8.1
- C++ 17

All execution times are in milliseconds.<br>
All accuracies are in %.<br>
All runs are in release mode.<br>
Use of the LBP and the L1 distance algorithms.<br>
14 histograms are generated from an image (divided by 3x3 then 2x2 then 1x1).<br>
No CNN is used.

### Sequential

<table>
	<thead>
		<tr>
			<th rowspan="2">Dataset</th>
			<th colspan="3">Training</th>
			<th colspan="6">Testing</th>
		</tr>
		<tr>
 			<th>Free exec</th>
			<th>Full exec</th>
			<th>Total exec</th>
			<th colspan="2">Free exec / Accuracy</th>
			<th colspan="2">Full exec / Accuracy</th>
			<th colspan="2">Total exec / Accuracy</th>
		</tr>
	</thead>
	<tbody>
		<tr>
			<td>100</td>
			<td>17,8</td>
			<td>18,4</td>
			<td>36,2</td>
			<td>38,7</td>
			<td>100</td>
			<td>30,4</td>
			<td>98</td>
			<td>69,1</td>
			<td>99</tr>
		<tr>
			<td>1000</td>
			<td>178</td>
			<td>179</td>
			<td>357</td>
			<td>2889</td>
			<td>99,9</td>
			<td>1619</td>
			<td>99,5</td>
			<td>4508</td>
			<td>99,7</tr>
		<tr>
			<td>10000</td>
			<td>1764</td>
			<td>1798</td>
			<td>3562</td>
			<td>270173</td>
			<td>100</td>
			<td>143559</td>
			<td>99,2</td>
			<td>413732</td>
			<td>99,6</tr>
	</tbody>
</table>

### Parallel

<table>
	<thead>
		<tr>
			<th rowspan="2">Dataset</th>
			<th colspan="3">Training</th>
			<th colspan="6">Testing</th>
		</tr>
		<tr>
 			<th>Free exec</th>
			<th>Full exec</th>
			<th>Total exec</th>
			<th colspan="2">Free exec / Accuracy</th>
			<th colspan="2">Full exec / Accuracy</th>
			<th colspan="2">Total exec / Accuracy</th>
		</tr>
	</thead>
	<tbody>
		<tr>
			<td>100</td>
			<td>7,84</td>
			<td>7,55</td>
			<td>15,39</td>
			<td>11,2</td>
			<td>100</td>
			<td>9,44</td>
			<td>98</td>
			<td>20,64</td>
			<td>99</tr>
		<tr>
			<td>1000</td>
			<td>34,8</td>
			<td>34,29</td>
			<td>69,09</td>
			<td>420</td>
			<td>99,9</td>
			<td>218</td>
			<td>99,5</td>
			<td>638</td>
			<td>99,7</tr>
		<tr>
			<td>10000</td>
			<td>276</td>
			<td>273</td>
			<td>549</td>
			<td>57052</td>
			<td>100</td>
			<td>33684</td>
			<td>99,2</td>
			<td>90736</td>
			<td>99,6</tr>
	</tbody>
</table>