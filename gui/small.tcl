router foo 0 0 simpletest
router bar 0 1 simpletest
router baz 1 0 simpletest
router quux 1 1 simpletest
PE first 0 0 simpletest
PE second 0 1 simpletest
link first foo
link foo first
link second bar
link bar second
link foo bar
link bar baz
link baz quux
link quux foo
