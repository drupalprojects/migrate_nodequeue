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
    // TODO
    dpm($d7queue, 'd');
    return array(1); // TODO or qid, depending on what the API returns
  }

  /**
   * @override
   *
   * Use nodequeue API.
   */
  public function rollback(array $key) {
    $qid = $key['destid'];
    nodequeue_delete($qid);
  }

}

