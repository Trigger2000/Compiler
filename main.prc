echo("Вычисление факторила. Введите число\n");

i = input();
result = 1;

if (i < 0)
{
	echo("Неправильный ввод!\n");
}
else
{
	while (i > 0)
	{
		result = result * i;
		i = i - 1;
	}
}
