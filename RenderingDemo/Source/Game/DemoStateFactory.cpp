#include "RenderingDemo_pch.h"
#include "DemoStateFactory.h"
#include "States\BoxTestState.h"

using namespace Anubis;

State * DemoStateFactory::VCreateState(AINT32 i32Id) const
{
	switch (i32Id)
	{
	case GS_BOXTEST:
		return new BoxTestState();
	};

	return StateFactory::VCreateState(i32Id);
}