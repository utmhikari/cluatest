function fib(n) 
    if n < 0 then
        return 0;
    elseif n == 0 or n == 1 then
        return 1;
    else
        return fib(n - 2) + fib(n - 1);
    end
end

print("Fib 5 is: %d", fib(5))