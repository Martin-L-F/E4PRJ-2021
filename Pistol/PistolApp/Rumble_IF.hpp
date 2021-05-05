class Rumble_IF
{
public:
	Rumble_IF();
	~Rumble_IF();

	void startRumble();

private:
	const char* _devPath = "/dev/RUMBLE";
};
