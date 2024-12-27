/* def foo(x of string) of int
	y = (a <= b) && (a < c) || (b > c) && (a >= c);
	z = (a != b) || (a == c);
	w = a ^ b & c | d;
	v = (-a) && (!b || ~c);
end */

/* def foo (x of int) of char
	if (x == 2) then
		if (y == 10) then
			z = 1;
		else 
			z = 2;		
	else z = 2 * 3;
	
	z = 1000;
end */

/* def foo (x of int) of char
	while x > 3 
		if x > 10 then
			y = 2;
		else y = 5;
		
		while y > 1 
			y = z / y;
		end
			
		x = x - 1;
	end
	w = x / y * z;
end */

/* def foo(x of int) of char
	x = 2;
	{
		x = x + 1;
		y = y * 2;
	} while x < 6;
	z = y / x;
end */

def foo_break(x of int)
	while x < 10
	{
		y = x * 2;
		if (y > 5) then
		{
			z = y;
			break;
		}
		else 
			z = x;
		
		y = y + 1;
		
		while y < 20 
			y = y * 2;
			if (y == 10) then break;
		end
		x = x + 1;
	}
	end
end