/** Copyright 2008, 2009, 2010, 2011, 2012 Roland Olbricht
*
* This file is part of Overpass_API.
*
* Overpass_API is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* Overpass_API is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Overpass_API.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <sstream>

#include "../../expat/expat_justparse_interface.h"
#include "../../template_db/random_file.h"
#include "../../template_db/transaction.h"
#include "../core/settings.h"
#include "../frontend/output.h"
#include "node_updater.h"


int main(int argc, char* args[])
{
  if (argc < 3)
  {
    std::cout<<"Usage: "<<args[0]<<" db_dir target [--index=INDEX]\n";
    return 0;
  }
  
  string db_dir(args[1]);

  uint32 index_int;
  bool index_used = false;
  if (argc >= 4)
  {
    index_used = true;
    std::string index_s(args[3]);
    if (index_s.size() >= 10 && index_s.substr(8, 2) == "0x")
    {
      std::istringstream in(&args[3][10]);
      in>>std::hex>>index_int;
    }
    else
    {
      std::istringstream in(&args[3][8]);
      in>>index_int;
    }
  }
  Uint31_Index index(index_int);
  
  try
  {    
    Nonsynced_Transaction transaction(false, false, db_dir, "");

    if (std::string("--nodes") == args[2])
    {
      Block_Backend< Uint31_Index, Node_Skeleton > db
          (transaction.data_index(osm_base_settings().NODES));
      for (Block_Backend< Uint31_Index, Node_Skeleton >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().id.val()<<'\n';
      }
    }
    else if (std::string("--nodes-meta") == args[2])
    {
      Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Node_Skeleton::Id_Type > > db
          (transaction.data_index(meta_settings().NODES_META));
      for (Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Node_Skeleton::Id_Type > >
               ::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().ref.val()<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--node-tags-local") == args[2])
    {
      Block_Backend< Tag_Index_Local, Node_Skeleton::Id_Type > db
          (transaction.data_index(osm_base_settings().NODE_TAGS_LOCAL));
      for (Block_Backend< Tag_Index_Local, Node_Skeleton::Id_Type >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().index<<'\t'
            <<dec<<it.object().val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\n';
      }
    }
    else if (std::string("--node-tags-global") == args[2])
    {
      Block_Backend< Tag_Index_Global, Tag_Object_Global< Node_Skeleton::Id_Type > > db
          (transaction.data_index(osm_base_settings().NODE_TAGS_GLOBAL));
      for (Block_Backend< Tag_Index_Global, Tag_Object_Global< Node_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.object().idx.val()<<'\t'
            <<dec<<it.object().id.val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\n';
      }
    }
    else if (std::string("--node-keys") == args[2])
    {
      Block_Backend< Uint32_Index, String_Object > db
          (transaction.data_index(osm_base_settings().NODE_KEYS));
      for (Block_Backend< Uint32_Index, String_Object >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
        cout<<dec<<it.index().val()<<'\t'<<it.object().val()<<'\n';
    }
    else if (std::string("--attic-node-idxs") == args[2])
    {
      Block_Backend< Node_Skeleton::Id_Type, Uint31_Index > db
          (transaction.data_index(attic_settings().NODE_IDX_LIST));
      for (Block_Backend< Node_Skeleton::Id_Type, Uint31_Index >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
        cout<<dec<<it.index().val()<<'\t'
            <<hex<<it.object().val()<<'\n';
    }
    else if (std::string("--attic-nodes") == args[2])
    {
      Block_Backend< Uint31_Index, Attic< Node_Skeleton > > db
          (transaction.data_index(attic_settings().NODES));
      for (Block_Backend< Uint31_Index, Attic< Node_Skeleton > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().id.val()<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--attic-nodes-meta") == args[2])
    {
      Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Node_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().NODES_META));
      for (Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Node_Skeleton::Id_Type > >
               ::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().ref.val()<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--attic-node-tags-local") == args[2])
    {
      Block_Backend< Tag_Index_Local, Attic< Node_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().NODE_TAGS_LOCAL));
      for (Block_Backend< Tag_Index_Local, Attic< Node_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().index<<'\t'
            <<dec<<it.object().val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--attic-node-tags-global") == args[2])
    {
      Block_Backend< Tag_Index_Global, Attic< Tag_Object_Global< Node_Skeleton::Id_Type > > > db
          (transaction.data_index(attic_settings().NODE_TAGS_GLOBAL));
      for (Block_Backend< Tag_Index_Global, Attic< Tag_Object_Global< Node_Skeleton::Id_Type > > >
               ::Flat_Iterator it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.object().idx.val()<<'\t'
            <<dec<<it.object().id.val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--nodes-undelete") == args[2])
    {
      Block_Backend< Uint32_Index, Attic< Node_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().NODES_UNDELETED));
      for (Block_Backend< Uint32_Index, Attic< Node_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().val()<<'\t'
            <<dec<<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--node-changelog") == args[2])
    {
      Block_Backend< Timestamp, Change_Entry< Node_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().NODE_CHANGELOG));
      for (Block_Backend< Timestamp, Change_Entry< Node_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<dec<<it.index().timestamp<<'\t'
            <<hex<<it.object().old_idx.val()<<'\t'<<it.object().new_idx.val()<<'\t'
            <<dec<<it.object().elem_id.val()<<'\n';
      }
    }
    else if (std::string("--ways") == args[2])
    {
      Block_Backend< Uint31_Index, Way_Skeleton > db
          (transaction.data_index(osm_base_settings().WAYS));
      if (index_used)
      {
	std::set< Uint31_Index > req;
	req.insert(index);
        for (Block_Backend< Uint31_Index, Way_Skeleton >::Discrete_Iterator
             it(db.discrete_begin(req.begin(), req.end())); !(it == db.discrete_end()); ++it)
        {
          cout<<hex<<it.index().val()<<'\t'
              <<dec<<it.object().id.val()<<'\n';
        }
      }
      else
      {
        for (Block_Backend< Uint31_Index, Way_Skeleton >::Flat_Iterator
             it(db.flat_begin()); !(it == db.flat_end()); ++it)
        {
          cout<<hex<<it.index().val()<<'\t'
              <<dec<<it.object().id.val()<<'\n';
        }
      }
    }
    else if (std::string("--ways-map") == args[2])
    {
      if (index_used)
      {
        Random_File< Uint31_Index > random(transaction.random_index(osm_base_settings().WAYS));
	std::cout<<"0x"<<std::hex<<random.get(index.val()).val()<<'\n';
      }
    }
    else if (std::string("--ways-meta") == args[2])
    {
      Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Way_Skeleton::Id_Type > > db
          (transaction.data_index(meta_settings().WAYS_META));
      for (Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Way_Skeleton::Id_Type > >
               ::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().ref.val()<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--way-tags-local") == args[2])
    {
      Block_Backend< Tag_Index_Local, Way_Skeleton::Id_Type > db
          (transaction.data_index(osm_base_settings().WAY_TAGS_LOCAL));
      for (Block_Backend< Tag_Index_Local, Way_Skeleton::Id_Type >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().index<<'\t'
            <<dec<<it.object().val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\n';
      }
    }
    else if (std::string("--way-tags-global") == args[2])
    {
      Block_Backend< Tag_Index_Global, Tag_Object_Global< Way_Skeleton::Id_Type > > db
          (transaction.data_index(osm_base_settings().WAY_TAGS_GLOBAL));
      for (Block_Backend< Tag_Index_Global, Tag_Object_Global< Way_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.object().idx.val()<<'\t'
            <<dec<<it.object().id.val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\n';
      }
    }
    else if (std::string("--way-keys") == args[2])
    {
      Block_Backend< Uint32_Index, String_Object > db
          (transaction.data_index(osm_base_settings().WAY_KEYS));
      for (Block_Backend< Uint32_Index, String_Object >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
        cout<<dec<<it.index().val()<<'\t'<<it.object().val()<<'\n';
    }
    else if (std::string("--attic-way-idxs") == args[2])
    {
      Block_Backend< Way_Skeleton::Id_Type, Uint31_Index > db
          (transaction.data_index(attic_settings().WAY_IDX_LIST));
      for (Block_Backend< Way_Skeleton::Id_Type, Uint31_Index >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
        std::cout<<"0x"<<std::dec<<it.index().val()<<'\t'
            <<"0x"<<std::hex<<it.object().val()<<'\n';
    }
    else if (std::string("--attic-ways") == args[2])
    {
      Block_Backend< Uint31_Index, Attic< Way_Skeleton > > db
          (transaction.data_index(attic_settings().WAYS));
      if (index_used)
      {
	std::set< Uint31_Index > req;
	req.insert(index);
        for (Block_Backend< Uint31_Index, Attic< Way_Skeleton > >::Discrete_Iterator
             it(db.discrete_begin(req.begin(), req.end())); !(it == db.discrete_end()); ++it)
        {
          cout<<hex<<it.index().val()<<'\t'
              <<dec<<it.object().id.val()<<'\t'
              <<it.object().timestamp<<'\n';
        }
      }
      else
      {
        for (Block_Backend< Uint31_Index, Attic< Way_Skeleton > >::Flat_Iterator
             it(db.flat_begin()); !(it == db.flat_end()); ++it)
        {
          cout<<hex<<it.index().val()<<'\t'
              <<dec<<it.object().id.val()<<'\t'
              <<it.object().timestamp<<'\n';
        }
      }
    }
    else if (std::string("--attic-ways-delta") == args[2])
    {
      Block_Backend< Uint31_Index, Attic< Way_Delta > > db
          (transaction.data_index(attic_settings().WAYS));
      if (index_used)
      {
	std::set< Uint31_Index > req;
	req.insert(index);
        for (Block_Backend< Uint31_Index, Attic< Way_Delta > >::Discrete_Iterator
             it(db.discrete_begin(req.begin(), req.end())); !(it == db.discrete_end()); ++it)
        {
          cout<<hex<<it.index().val()<<'\t'
              <<dec<<it.object().id.val()<<'\t'<<it.object().full<<'\t'
              <<it.object().timestamp<<'\n';
        }
      }
      else
      {
        for (Block_Backend< Uint31_Index, Attic< Way_Delta > >::Flat_Iterator
             it(db.flat_begin()); !(it == db.flat_end()); ++it)
        {
          cout<<hex<<it.index().val()<<'\t'
              <<dec<<it.object().id.val()<<'\t'<<it.object().full<<'\t'
              <<it.object().timestamp<<'\n';
        }
      }
    }
    else if (std::string("--attic-ways-map") == args[2])
    {
      if (index_used)
      {
        Random_File< Uint31_Index > random(transaction.random_index(attic_settings().WAYS));
	std::cout<<"0x"<<std::hex<<random.get(index.val()).val()<<'\n';
      }
    }
    else if (std::string("--attic-ways-idxs") == args[2])
    {
      Block_Backend< Way::Id_Type, Uint31_Index > db
          (transaction.data_index(attic_settings().WAY_IDX_LIST));
      if (index_used)
      {
	std::set< Way::Id_Type > req;
	req.insert(index);
        for (Block_Backend< Way::Id_Type, Uint31_Index >::Discrete_Iterator
             it(db.discrete_begin(req.begin(), req.end())); !(it == db.discrete_end()); ++it)
        {
          cout<<dec<<it.index().val()<<'\t'
              <<hex<<it.object().val()<<'\n';
        }
      }
      else
      {
        for (Block_Backend< Way::Id_Type, Uint31_Index >::Flat_Iterator
             it(db.flat_begin()); !(it == db.flat_end()); ++it)
        {
          cout<<dec<<it.index().val()<<'\t'
              <<hex<<it.object().val()<<'\n';
        }
      }
    }
    else if (std::string("--attic-ways-meta") == args[2])
    {
      Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Way_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().WAYS_META));
      for (Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Way_Skeleton::Id_Type > >
               ::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().ref.val()<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--attic-way-tags-local") == args[2])
    {
      Block_Backend< Tag_Index_Local, Attic< Way_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().WAY_TAGS_LOCAL));
      for (Block_Backend< Tag_Index_Local, Attic< Way_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().index<<'\t'
            <<dec<<it.object().val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--attic-way-tags-global") == args[2])
    {
      Block_Backend< Tag_Index_Global, Attic< Tag_Object_Global< Way_Skeleton::Id_Type > > > db
          (transaction.data_index(attic_settings().WAY_TAGS_GLOBAL));
      for (Block_Backend< Tag_Index_Global, Attic< Tag_Object_Global< Way_Skeleton::Id_Type > > >
               ::Flat_Iterator it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.object().idx.val()<<'\t'
            <<dec<<it.object().id.val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--ways-undelete") == args[2])
    {
      Block_Backend< Uint31_Index, Attic< Way_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().WAYS_UNDELETED));
      for (Block_Backend< Uint31_Index, Attic< Way_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().val()<<'\t'
            <<dec<<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--way-changelog") == args[2])
    {
      Block_Backend< Timestamp, Change_Entry< Way_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().WAY_CHANGELOG));
      for (Block_Backend< Timestamp, Change_Entry< Way_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<dec<<it.index().timestamp<<'\t'
            <<hex<<it.object().old_idx.val()<<'\t'<<it.object().new_idx.val()<<'\t'
            <<dec<<it.object().elem_id.val()<<'\n';
      }
    }
    else if (std::string("--rels") == args[2])
    {
      Block_Backend< Uint31_Index, Relation_Skeleton > db
          (transaction.data_index(osm_base_settings().RELATIONS));
      for (Block_Backend< Uint31_Index, Relation_Skeleton >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().id.val()<<'\n';
      }
    }
    else if (std::string("--rel-roles") == args[2])
    {
      Block_Backend< Uint32_Index, String_Object > db
          (transaction.data_index(osm_base_settings().RELATION_ROLES));
      for (Block_Backend< Uint32_Index, String_Object >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
        cout<<dec<<it.index().val()<<'\t'<<it.object().val()<<'\n';
    }
    else if (std::string("--rels-meta") == args[2])
    {
      Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Relation_Skeleton::Id_Type > > db
          (transaction.data_index(meta_settings().RELATIONS_META));
      for (Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Relation_Skeleton::Id_Type > >
               ::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().ref.val()<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--rel-tags-local") == args[2])
    {
      Block_Backend< Tag_Index_Local, Relation_Skeleton::Id_Type > db
          (transaction.data_index(osm_base_settings().RELATION_TAGS_LOCAL));
      for (Block_Backend< Tag_Index_Local, Relation_Skeleton::Id_Type >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().index<<'\t'
            <<dec<<it.object().val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\n';
      }
    }
    else if (std::string("--rel-tags-global") == args[2])
    {
      Block_Backend< Tag_Index_Global, Tag_Object_Global< Relation_Skeleton::Id_Type > > db
          (transaction.data_index(osm_base_settings().RELATION_TAGS_GLOBAL));
      for (Block_Backend< Tag_Index_Global, Tag_Object_Global< Relation_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.object().idx.val()<<'\t'
            <<dec<<it.object().id.val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\n';
      }
    }
    else if (std::string("--rel-keys") == args[2])
    {
      Block_Backend< Uint32_Index, String_Object > db
          (transaction.data_index(osm_base_settings().RELATION_KEYS));
      for (Block_Backend< Uint32_Index, String_Object >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
        cout<<dec<<it.index().val()<<'\t'<<it.object().val()<<'\n';
    }
    else if (std::string("--attic-rel-idxs") == args[2])
    {
      Block_Backend< Relation_Skeleton::Id_Type, Uint31_Index > db
          (transaction.data_index(attic_settings().RELATION_IDX_LIST));
      for (Block_Backend< Relation_Skeleton::Id_Type, Uint31_Index >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
        cout<<dec<<it.index().val()<<'\t'
            <<hex<<it.object().val()<<'\n';
    }
    else if (std::string("--attic-rels") == args[2])
    {
      Block_Backend< Uint31_Index, Attic< Relation_Skeleton > > db
          (transaction.data_index(attic_settings().RELATIONS));
      for (Block_Backend< Uint31_Index, Attic< Relation_Skeleton > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().id.val()<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--attic-rels-delta") == args[2])
    {
      Block_Backend< Uint31_Index, Attic< Relation_Delta > > db
          (transaction.data_index(attic_settings().RELATIONS));
      for (Block_Backend< Uint31_Index, Attic< Relation_Delta > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().id.val()<<'\t'<<it.object().full<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--attic-rels-meta") == args[2])
    {
      Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Relation_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().RELATIONS_META));
      for (Block_Backend< Uint31_Index, OSM_Element_Metadata_Skeleton< Relation_Skeleton::Id_Type > >
               ::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().val()<<'\t'
            <<dec<<it.object().ref.val()<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--attic-rel-tags-local") == args[2])
    {
      Block_Backend< Tag_Index_Local, Attic< Relation_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().RELATION_TAGS_LOCAL));
      for (Block_Backend< Tag_Index_Local, Attic< Relation_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.index().index<<'\t'
            <<dec<<it.object().val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--attic-rel-tags-global") == args[2])
    {
      Block_Backend< Tag_Index_Global, Attic< Tag_Object_Global< Relation_Skeleton::Id_Type > > > db
          (transaction.data_index(attic_settings().RELATION_TAGS_GLOBAL));
      for (Block_Backend< Tag_Index_Global, Attic< Tag_Object_Global< Relation_Skeleton::Id_Type > > >
               ::Flat_Iterator it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<hex<<it.object().idx.val()<<'\t'
            <<dec<<it.object().id.val()<<'\t'
            <<it.index().key<<'\t'<<it.index().value<<'\t'
            <<it.object().timestamp<<'\n';
      }
    }
    else if (std::string("--rel-changelog") == args[2])
    {
      Block_Backend< Timestamp, Change_Entry< Relation_Skeleton::Id_Type > > db
          (transaction.data_index(attic_settings().RELATION_CHANGELOG));
      for (Block_Backend< Timestamp, Change_Entry< Relation_Skeleton::Id_Type > >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
      {
        cout<<dec<<it.index().timestamp<<'\t'
            <<hex<<it.object().old_idx.val()<<'\t'<<it.object().new_idx.val()<<'\t'
            <<dec<<it.object().elem_id.val()<<'\n';
      }
    }
    else if (std::string("--user") == args[2])
    {
      Block_Backend< Uint32_Index, Uint31_Index > db
          (transaction.data_index(meta_settings().USER_INDICES));
      for (Block_Backend< Uint32_Index, Uint31_Index >::Flat_Iterator
           it(db.flat_begin()); !(it == db.flat_end()); ++it)
        cout<<dec<<it.index().val()<<'\t'<<hex<<it.object().val()<<'\n';
    }
    else
      std::cout<<"Unknown target.\n";
  }
  catch (File_Error e)
  {
    std::cerr<<e.origin<<' '<<e.filename<<' '<<e.error_number<<'\n';
  }
  
  return 0;
}
