player = {
	position = {
		x = 20,
		y = 90
	},
	values = { 0, 1, 2, 3, 4, 5, 6 },
	name = 'player',
	model = 'warrior.obj'
}

array = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }

function sum(x, y)
	return x + y
end

function oneArgument(a)
	-- do nothing
end

function oneReturn()
	return 1
end

function oneArgumentOneReturn(x)
	return x
end

function foo()
end

function add(a, b)
   return a + b
end

function sum_and_difference(a, b)
   return (a+b), (a-b);
end

function bar()
   return 4, true, "hi"
end

function execute()
   return cadd(5, 6);
end

function doozy(a)
 	x, y = doozy_c(a, 2 * a)
 	return x * y
end
