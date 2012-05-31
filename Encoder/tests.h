#pragma once

class Friendly
{
public:
	void static test_formCollectionOfAvailableKeys();
	void static test_Encoder_keyForThread();
	void static test_Encoder_generateKeyForThread();
	void static test_Encoder_renameKey();
	void static test_MapEncoder_costructor();	
	void static test_EncodedDataAccessor_getNext();
	void static test_LengthProducer_constructor();
	void static test_LengthProducer_getNext();
};


void beginTests();