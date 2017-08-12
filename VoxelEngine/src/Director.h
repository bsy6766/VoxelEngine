#ifndef DIRECTOR_H
#define DIRECTOR_H

/*
*	@class Director
*	@brief Manages scene
*	@note singleton
*/
class Director
{
private:
	// Delete copy, move, assign operators
	Director(Director const&) = delete;             // Copy construct
	Director(Director&&) = delete;                  // Move construct
	Director& operator=(Director const&) = delete;  // Copy assign
	Director& operator=(Director &&) = delete;      // Move assign
public:
	static Director& getInstance()
	{
		static Director instance;
		return instance;
	}

	void nextScene();
	void update();
	void end();
protected:
	Director();
	~Director();
};

#endif