
class Registry {
public:
	Registry(void);

	void read_reg(void);
	void write_reg(void);

	// Window locations and sizes
	int main_x, main_y, main_dx, main_dy;
	int tablemap_x, tablemap_y, tablemap_dx, tablemap_dy;
	int grhash_x, grhash_y, grhash_dx, grhash_dy;

};

