#include "cube.h"
#include "entityscript/scriptmanager.h"
#include "entityscript/instance.h"

////Test Script
//test_def test tname [
//	//buildt in signals
//	connect "default" [ echo execute state default, arg: (param 0)]
//	connect "waiting" [ echo EMMIT ARGTEST; emmit argtest 10 lala 20 10; echo END]
//
//	//own signals
//	callback "argtest" "isi"
//	connect "argtest" [ echo callback argtest - args are (param 0) (param 1) (param 2) ]
//]
//
//test_instance tname


namespace EntityScript
{
	class InstanceTest : public Instance
	{
	public:
		InstanceTest(Script& s) : Instance(s) {}
		~InstanceTest() {}

		void process() {}

		void debug()
		{
			conoutf("InstanceTest");
		}
	};

	vector<Instance*> instances;
	ScriptManager manager;

	void TestInit()
	{
		manager.Register<InstanceTest>("test");
	}

	void TestNewInstance(const char* s)
	{
		Instance* test = manager.Create("test", s);
		if ( test ) {
			test->debug();
			for(int i = 0; i < 10; i++)
				test->process();
		}
	}

	ICOMMAND(test_entityscript, "", (), TestInit());
	ICOMMAND(test_def, "sss", (char* type, char* name, char* body), manager.newscript(type, name, body));
	ICOMMAND(test_instance, "s", (char* s), TestNewInstance(s));
}
