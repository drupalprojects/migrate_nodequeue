<?php

/**
 * Destination handler for {nodequeue_queue} and {nodequeue_subqueue}.
 *
 * TODO do we need to extend MigrateDestinationTable ?
 */
class NodequeueNodeDestination extends MigrateDestinationTable {
  public function __construct() {
    parent::__construct('nodequeue_queue');
  }

  /**
   * {nodequeue_nodes} data are uniquely identified by qid, sqid and nid.
   */
  public static function getKeySchema() {
    return array(
      'qid' => array(
        'description' => 'Queue id',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
      'sqid' => array(
        'description' => 'Subqueue this node is in',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
      'nid' => array(
        'description' => 'Node id in this subqueue',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => FALSE,
      ),
    );
  }

  /**
   * @override
   */
  public function import(stdClass $rowD7, stdclass $rowD6) {
    // TODO use nodequeue_subqueue_add ?
    $status = drupal_write_record($this->tableName, $rowD7);
    ($status == SAVED_NEW) ? $this->numCreated++ : $this->numUpdated++;
    return array($rowD7->qid, $rowD7->sqid, $rowD7->nid);
  }

  /**
   * @override
   */
  public function rollback(array $key) {
    // TODO nodequeue_node_delete ?
    db_delete($this->tableName)
      ->condition('qid',  $key['destid1'])
      ->condition('sqid', $key['destid2'])
      ->condition('nid',  $key['destid3'])
      ->execute();
  }

}

