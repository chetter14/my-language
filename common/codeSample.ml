// check function signature
def func_sign(a of int, b of string, c of long array[10], d of bool array[5]array[5]array[5]) of byte
	x = 2;
end

// check if statements
def if_stat()
	if (a > 3) then
		b = "ehy";
	
	if c < 5 then
		d = 0b1100;
	else
		d = 0xFFFF;
end

// check while statements
def while_stat()
	while x > 2
		y = y + 1;
	end
end

// check repeat statements
def repeat_stat()
	{
		x = 5 * x;
		if x > 50 then break;
	} until x < 100;

	y = 2 + y;
	until y < 50;
	
	i = 0;
	{
		j = 0;
		{
			j = j + 1;
		} while j < i;
	} while i < 10;
end

// check expressions
def foo_expr()
	a = (b + c) * d;
	a = b + c * d;
	a = b * c + d;
	a = b > c && d < e;
	a = -b;
	a = !c;
	
	a = b && c = d;
	
	a = x(arg);
	a = x(2)("hello", 3 + 3);
	
	y = a[1];
	y = a[1][1];
	
	c = x[1] + a[2];
	
	a = x[1, 3..5];
	
	z = x()[0];
	w = b[0][0]()()[0]();
end

def check_logic_and_bit()
	x = a | ~b & c ^ d;
	y = !a && b || c;
end