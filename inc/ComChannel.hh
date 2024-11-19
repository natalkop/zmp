#ifndef COMCHANNEL_HH
#define COMCHANNEL_HH

#include "AbstractComChannel.hh"

class ComChannel: public AbstractComChannel {
	private:
		int Socket;
	public:
		virtual void Init(int Socket) override;
		virtual int GetSocket() const override;
		virtual void LockAccess() override;
		virtual void UnlockAccess() override;
		virtual std::mutex &UseGuard() override;

};

#endif
