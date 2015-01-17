#pragma once

#include <Artemis\Artemis.h>

namespace artemis {
	class ProcessingSystem : public artemis::EntityProcessingSystem {
	private:
		virtual void processEntities (artemis::ImmutableBag <artemis::Entity*> & bag) {doProcessing();}
		virtual void processEntity (artemis::Entity & e) {}
	protected:
		virtual void doProcessing () = 0;
	};
}