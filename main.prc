from = 0; to = 1000;
echo("Задумай число от ",from," до ",to,", а я буду угадывать\n");

while (from <= to) 
{
   guess = (from+to) / 2;
   echo("Это ",guess,"?  (1=меньше, 2=больше, 3=попал) ");
   i = input();
   if (i == 1)
      to = guess-1;
   else if (i == 2)
      from = guess+1;
   else if (i == 3) 
   {
      echo("Ура! Я молодец!\n");
      exit;
   } 
   else
      echo("Я ничего не понял!\n");
}

echo("Врёшь, так не бывает!\n");
