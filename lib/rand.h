int g_rand_seed;
int srand(int seed)
{
	g_rand_seed=seed;
}
int rand()
{
	g_rand_seed=g_rand_seed*214013 +2531011;
	g_rand_seed=and(g_rand_seed/65536,32767);
	return g_rand_seed;
}

