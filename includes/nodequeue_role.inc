<?php

/**
 * Destination handler for nodequeues.
 */
class NodequeueRoleDestination extends MigrateDestinationTable {
  public function __construct() {
    parent::__construct('nodequeue_role');
  }

  /**
   * {nodequeue_roles} has no primary key so we define it excplicitely here.
   */
  public static function getKeySchema() {
    return array(
      'qid' => array(
        'description' => 'Queue id',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
      'rid' => array(
        'description' => 'Role id',
        'type' => 'int',
        'unsigned' => TRUE,
        'not null' => TRUE,
      ),
    );
  }

  /**
   * @override
   */
  public function import(stdClass $d7role, stdclass $d6role) {
    // Make sure D7 queue exists.
    // TODO

    // Make sure D7 role  exists.
    if (!user_role_load($d7role->rid)) {
      throw new MigrateException(t('Invalid destination role ID @d7rid (source role ID was @d6rid)', array('@d6rid' => $d6role->rid, '@d7rid' => $d7role->rid)));
    }

    /*
    $queue = nodequeue_load($d7content->qid);
    $subqueue = nodequeue_load_subqueue($d7content->sqid);
    nodequeue_subqueue_add($queue, $subqueue, $d7content->nid);
    $this->numCreated++;
    */
    // TODO import record
    return array($d7role->qid, $d7role->rid);
  }

  /**
   * @override
   *
   * TODO remove record
   */
  public function rollback(array $key) {
  }

}

