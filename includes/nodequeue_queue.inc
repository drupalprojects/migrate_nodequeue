<?php

/**
 * Destination handler for {nodequeue_queue} and {nodequeue_subqueue}.
 */
class NodequeueQueueDestination extends MigrateDestinationTable {
  public function __construct() {
    parent::__construct('nodequeue_queue');
  }

  /**
   * Get schema of {nodequeue_queue}.
   */
  public static function getKeySchema() {
    return parent::getKeySchema('nodequeue_queue'); // TODO need parent ?
  }

  /**
   * @override
   *
   * Use nodequeue API.
   */
  public function import(stdClass $d7queue, stdclass $d6queue) {
    $qid = nodequeue_save($d7queue);
    $this->numCreated++;
    return array($qid);
  }

  /**
   * @override
   *
   * Use nodequeue API.
   */
  public function rollback(array $key) {
    $qid = $key['destid1'];
    nodequeue_delete($qid);
  }

}

