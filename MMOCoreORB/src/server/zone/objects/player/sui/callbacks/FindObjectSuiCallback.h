
#ifndef FINDOBJECTSUICALLBACK_H_
#define FINDOBJECTSUICALLBACK_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"


class FindObjectSuiCallback : public SuiCallback {

public:
	FindObjectSuiCallback(ZoneServer* serv) : SuiCallback(serv) {

	}

	void run(CreatureObject* creature, SuiBox* sui, bool cancelPressed, Vector<UnicodeString>* args) {
		if (!sui->isListBox() || cancelPressed || server == NULL)
			return;

		SuiListBox* listBox = cast<SuiListBox*>( sui);

		if(!creature->isPlayerCreature())
			return;

		bool otherPressed = Bool::valueOf(args->get(0).toString());

		int index = Integer::valueOf(args->get(1).toString());

		if(index < 0)
			return;

		uint64 objectID = listBox->getMenuObjectID(index);

		ManagedReference<SceneObject*> object = server->getObject(objectID);

		if (object == NULL)
			return;

		if(otherPressed) {
			StringBuffer arguments;

			Locker locker(object, creature);

			if (object->getParent().get() != NULL) {
				arguments << String::valueOf(object->getPositionX()) << " " << String::valueOf(object->getPositionY());
				arguments << " " << object->getZone()->getZoneName();
				arguments << " " << String::valueOf(object->getPositionZ());
				arguments << " " << String::valueOf(object->getParentID());
			} else {
				arguments << String::valueOf(object->getWorldPositionX()) << " " << String::valueOf(object->getWorldPositionY());
				arguments << " " << object->getZone()->getZoneName();
			}

			ManagedReference<ObjectController*> objectController = server->getObjectController();
			objectController->activateCommand(creature, String("teleport").hashCode(), 0, 0, arguments.toString());

		} else {
			Reference<PlayerObject*> ghost = creature->getSlottedObject("ghost").castTo<PlayerObject*>();
			ManagedReference<WaypointObject*> obj = server->createObject(0xc456e788, 1).castTo<WaypointObject*>();
			obj->setPlanetCRC(object->getPlanetCRC());
			obj->setPosition(object->getWorldPositionX(), 0, object->getWorldPositionY());

			obj->setColor(WaypointObject::COLOR_GREEN);
			obj->setCustomObjectName(object->getDisplayedName(), false);

			obj->setActive(true);

			ghost->addWaypoint(obj, false, true);
		}
	}
};

#endif /* FINDOBJECTSUICALLBACK_H_ */
