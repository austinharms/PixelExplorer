namespace pixelexplorer {
	Timer::Timer() : _startTime(ClockType::now()) {}

	void Timer::reset() {
		_startTime = ClockType::now();
	}

	double Timer::elapsed() const {
		return std::chrono::duration_cast<SecondDuration>(ClockType::now() - _startTime).count();
	}
}