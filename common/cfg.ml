/* def foo(x of string) of int
	y = (a <= b) && (a < c) || (b > c) && (a >= c);
	z = (a != b) || (a == c);
	w = a ^ b & c | d;
	v = (-a) && (!b || ~c);
end */

def foo (x of int) of char
	x = 2;
	{
		x = x + 1;
		y = y * 2;
	} while x < 6;
	z = y / x;
end