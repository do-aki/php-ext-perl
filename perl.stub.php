<?php

/**
 * @generate-function-entries
 * @generate-class-entries
 */

class PerlException extends Exception {}

class Perl
{
    public function __construct(?string $perlClass = null, ?string $constructor = "new", mixed ...$constructorArgs) {}

    /**
     * Loads and executes a Perl file.
     *
     * @throws PerlException
     */
    public function require(string $file) : void {}

    /**
     * Evaluates Perl code and returns the result.
     *
     * @throws PerlException
     */
    public function eval(string $code) : mixed {}
}