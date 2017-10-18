//Const
//Ko dc thay doi tai thoi diem compile
int main() {
	const int i = 10;
	// i = 9; NG

	const int *p1 = &i; 	// data ko doi, pointer co the doi
	p1 ++;

	int* const p2;	//pointer la const, data co the doi

	const int* const p3; //pointer va data la const

	int const *p4 = &i;

//Neu const o ben trai dau *, data la const
//Neu const o ben phai dau *, pointer la const

	//cast
	const int i = 6;// thuc su muon doi no
	const_cast<int&> (i) = 10;

	int j = 10; //doi thanh const
	static_cast<const int&>(j) = 8;
/*
Why we use const
	1. De de phong thay doi gia tri cua bien (vo tinh)
	2. De hieu
	3. Compiler toi uu code, chay nhanh hon
	4. const means the variable can be put in to ROM (Read Only Memory - Embedded programming)
*/
}
