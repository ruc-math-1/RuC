ПУСТО ф(ВЕЩ п1[], ВЕЩ *п2, ВЕЩ п3[])
{
    assert(п1[0] == 1.0, "п1[0] must be 1.0");
    assert(п1[1] == 2.0, "п1[1] must be 2.0");

    assert(*п2 == 1.0, "*п2 must be 1.0");

    assert(п3[0] == 13.0, "п3[0] must be 13.0");
    assert(п3[1] == 14.0, "п3[1] must be 14.0");

    ВОЗВРАТ;
}

void main()
{
    ВЕЩ мас1[] = {1, 2}, мас2[2][] = {{11, 12}, {13,14}};
    ф(мас1, &мас1[0], мас2[1]);
}

