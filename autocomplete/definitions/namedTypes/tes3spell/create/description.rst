Create a new `tes3spell`_ object which will be stored as part of the current saved game.

The example below creates a spell object.
.. code-block:: lua
	:linenos:

local spellId = "_dynamic_spell_id"
local spellName = "_dynamic_spell_name"

local spell = tes3spell.create(spellId, spellName)

-- effects can be set later on using spell.effects table
